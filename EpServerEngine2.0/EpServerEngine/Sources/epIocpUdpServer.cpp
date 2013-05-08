/*! 
IocpUdpServer for the EpServerEngine
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
#include "epIocpUdpServer.h"
#include "epIocpUdpSocket.h"
#include "epIocpServerProcessor.h"
#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;


IocpUdpServer::IocpUdpServer(epl::LockPolicy lockPolicyType):BaseUdpServer(lockPolicyType)
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


IocpUdpServer::IocpUdpServer(const IocpUdpServer& b):BaseUdpServer(b)
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

IocpUdpServer::~IocpUdpServer()
{
	if(m_workerLock)
		EP_DELETE m_workerLock;
}

IocpUdpServer & IocpUdpServer::operator=(const IocpUdpServer&b)
{
	if(this!=&b)
	{
		BaseUdpServer::operator =(b);
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

void IocpUdpServer::CallBackFunc(BaseWorkerThread *p)
{
	epl::LockObj lock(m_workerLock);
	m_emptyWorkerList.push(p);
}
void IocpUdpServer::pushJob(BaseJob * job)
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

void IocpUdpServer::StopServer()
{
	BaseUdpServer::StopServer();

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

bool IocpUdpServer::StartServer(const ServerOps &ops)
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
	
	return BaseUdpServer::StartServer(ops);
}

void IocpUdpServer::execute()
{
	Packet recvPacket(NULL,m_maxPacketSize);
	char *packetData=const_cast<char*>(recvPacket.GetPacket());
	int length=recvPacket.GetPacketByteSize();
	sockaddr clientSockAddr;
	int sockAddrSize=sizeof(sockaddr);
	while(m_listenSocket!=INVALID_SOCKET)
	{
		int recvLength=recvfrom(m_listenSocket,packetData,length, 0,&clientSockAddr,&sockAddrSize);

		IocpUdpSocket *workerObj=(IocpUdpSocket*)m_socketList.Find(clientSockAddr,socketCompare);
		if(workerObj)
		{
			if(recvLength<=0)
			{
				Packet *passPacket=EP_NEW Packet(packetData,0);
				workerObj->addPacket(passPacket);
				passPacket->ReleaseObj();
				continue;
			}	
			Packet *passPacket=EP_NEW Packet(packetData,recvLength);
			workerObj->addPacket(passPacket);
			passPacket->ReleaseObj();
		}
		else
		{
			if(recvLength<=0)
				continue;
			if(GetMaximumConnectionCount()!=CONNECTION_LIMIT_INFINITE)
			{
				if(m_socketList.Count()>=GetMaximumConnectionCount())
				{
					continue;
				}
			}
			if(!m_callBackObj->OnAccept(clientSockAddr))
			{
				continue;
			}
			/// Create Worker Thread
			Packet *passPacket=EP_NEW Packet(packetData,recvLength);
			IocpUdpSocket *accWorker=EP_NEW IocpUdpSocket(m_callBackObj,m_waitTime,m_lockPolicy);
			accWorker->setSockAddr(clientSockAddr);
			accWorker->setOwner(this);
			accWorker->setMaxPacketByteSize(m_maxPacketSize);
			m_socketList.Push(accWorker);
			accWorker->Start();
			accWorker->addPacket(passPacket);
			accWorker->ReleaseObj();
			passPacket->ReleaseObj();

		}

	}

	stopServer();
} 

