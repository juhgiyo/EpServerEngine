/*! 
IocpTcpServer for the EpServerEngine
Copyright (C) 2012  Woong Gyu La <juhgiyo@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "epIocpTcpServer.h"
#include "epIocpTcpSocket.h"
#include "epIocpTcpProcessor.h"
#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;


IocpTcpServer::IocpTcpServer(epl::LockPolicy lockPolicyType):BaseTcpServer(lockPolicyType)
{
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_workerLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_workerLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_workerLock=EP_NEW epl::NoLock();
		break;
	default:
		m_workerLock=NULL;
		break;
	}
}


IocpTcpServer::IocpTcpServer(const IocpTcpServer& b):BaseTcpServer(b)
{
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_workerLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_workerLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_workerLock=EP_NEW epl::NoLock();
		break;
	default:
		m_workerLock=NULL;
		break;
	}
	LockObj lock(b.m_baseServerLock);
}

IocpTcpServer::~IocpTcpServer()
{
	if(m_workerLock)
		EP_DELETE m_workerLock;
}

IocpTcpServer & IocpTcpServer::operator=(const IocpTcpServer&b)
{
	if(this!=&b)
	{
		BaseTcpServer::operator =(b);
		switch(m_lockPolicy)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_workerLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_workerLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_workerLock=EP_NEW epl::NoLock();
			break;
		default:
			m_workerLock=NULL;
			break;
		}
		LockObj lock(b.m_baseServerLock);

	}
	return *this;
}

void IocpTcpServer::CallBackFunc(BaseWorkerThread *p)
{
	epl::LockObj lock(m_workerLock);
	m_emptyWorkerList.push(p);
}
void IocpTcpServer::pushJob(BaseJob * job)
{
	epl::LockObj lock(m_workerLock);
	if(m_emptyWorkerList.size())
	{
		m_emptyWorkerList.front()->Push(job);
		m_emptyWorkerList.pop();
	}
	else
	{
		if(!m_workerList.size())
		{
			return;
		}

		size_t jobCount=m_workerList.at(0)->GetJobCount();
		int workerIdx=0;	
		
		for(int trav=1;trav<m_workerList.size();trav++)
		{
			if(m_workerList.at(trav)->GetJobCount()<jobCount)
			{
				jobCount=m_workerList.at(trav)->GetJobCount();
				workerIdx=trav;
			}
		}
		m_workerList.at(workerIdx)->Push(job);
	}
}

void IocpTcpServer::StopServer()
{
	epl::LockObj lock(m_workerLock);
	BaseTcpServer::StopServer();

	while(!m_emptyWorkerList.empty())
		m_emptyWorkerList.pop();

	for(int trav=0;trav<m_workerList.size();trav++)
	{
		m_workerList.at(trav)->TerminateWorker(m_waitTime);
		EP_DELETE m_workerList.at(trav);
	}
	m_workerList.clear();
}

bool IocpTcpServer::StartServer(const ServerOps &ops)
{
	epl::LockObj lock(m_workerLock);
	int workerCount=ops.workerThreadCount;
	if(workerCount==0)
	{
		workerCount=System::GetNumberOfCores()*2;
	}
	for(int trav=0;trav<workerCount;trav++)
	{
		BaseWorkerThread *workerThread=WorkerThreadFactory::GetWorkerThread(BaseWorkerThread::THREAD_LIFE_SUSPEND_AFTER_WORK);
		
		workerThread->SetCallBackClass(this);

		m_workerList.push_back(workerThread);
		m_emptyWorkerList.push(workerThread);
		workerThread->SetJobProcessor(EP_NEW IocpTcpProcessor());
		workerThread->Start();
	}
	
	
	return BaseTcpServer::StartServer(ops);
}

void IocpTcpServer::execute()
{
	SOCKET clientSocket;
	sockaddr sockAddr;
	int sizeOfSockAddr=sizeof(sockaddr);
	while(1)
	{
		clientSocket=accept(m_listenSocket,&sockAddr,&sizeOfSockAddr);
		if(clientSocket == INVALID_SOCKET || m_listenSocket== INVALID_SOCKET)
		{
			break;			
		}
		else
		{
			if(!m_callBackObj->OnAccept(sockAddr))
			{
				closesocket(clientSocket);
				continue;
			}
			IocpTcpSocket *accWorker=EP_NEW IocpTcpSocket(m_callBackObj,m_waitTime,m_lockPolicy);
			accWorker->setClientSocket(clientSocket);
			accWorker->setSockAddr(sockAddr);

			accWorker->setOwner(this);
			m_socketList.Push(accWorker);	
			accWorker->Start();
			accWorker->ReleaseObj();
			if(GetMaximumConnectionCount()!=CONNECTION_LIMIT_INFINITE)
			{
				while(m_socketList.Count()>=GetMaximumConnectionCount())
				{
					m_socketList.WaitForListSizeDecrease();
				}
			}

		}
	}

	stopServer();
} 

