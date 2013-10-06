/*! 
IocpTcpServer for the EpServerEngine

The MIT License (MIT)

Copyright (c) 2012-2013 Woong Gyu La <juhgiyo@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "epIocpTcpServer.h"
#include "epIocpTcpSocket.h"
#include "epIocpServerProcessor.h"
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
	BaseTcpServer::StopServer();

	m_workerLock->Lock();
	while(!m_emptyWorkerList.empty())
		m_emptyWorkerList.pop();

	for(int trav=0;trav<m_workerList.size();trav++)
	{
		m_workerList.at(trav)->TerminateWorker(m_waitTime);
		EP_DELETE m_workerList.at(trav);
	}
	m_workerList.clear();
	m_workerLock->Unlock();
}

bool IocpTcpServer::StartServer(const ServerOps &ops)
{
	m_workerLock->Lock();
	while(!m_emptyWorkerList.empty())
		m_emptyWorkerList.pop();

	for(int trav=0;trav<m_workerList.size();trav++)
	{
		m_workerList.at(trav)->TerminateWorker(m_waitTime);
		EP_DELETE m_workerList.at(trav);
	}
	m_workerList.clear();

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
		workerThread->SetJobProcessor(EP_NEW IocpServerProcessor());
		workerThread->Start();
	}
	m_workerLock->Unlock();
	
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
			if(!accWorker)
			{
				closesocket(clientSocket);
				continue;
			}
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

