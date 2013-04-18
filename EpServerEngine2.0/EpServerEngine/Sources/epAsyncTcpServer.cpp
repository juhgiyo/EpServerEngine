/*! 
AsyncTcpServer for the AsyncTcpServer
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
#include "epAsyncTcpServer.h"
#include "epAsyncTcpSocket.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

AsyncTcpServer::AsyncTcpServer(epl::LockPolicy lockPolicyType):BaseTcpServer(lockPolicyType)
{
	m_isAsynchronousReceive=true;
}


AsyncTcpServer::AsyncTcpServer(const AsyncTcpServer& b):BaseTcpServer(b)
{
	LockObj lock(b.m_baseServerLock);
	m_isAsynchronousReceive=b.m_isAsynchronousReceive;
}

AsyncTcpServer::~AsyncTcpServer()
{
}

AsyncTcpServer & AsyncTcpServer::operator=(const AsyncTcpServer&b)
{
	if(this!=&b)
	{
		BaseTcpServer::operator =(b);
		LockObj lock(b.m_baseServerLock);
		m_isAsynchronousReceive=b.m_isAsynchronousReceive;
	}
	return *this;
}

bool AsyncTcpServer::GetIsAsynchronousReceive() const
{
	return m_isAsynchronousReceive;
}
void AsyncTcpServer::SetIsAsynchronousReceive(bool isASynchronousReceive)
{
	m_isAsynchronousReceive=isASynchronousReceive;
}

bool AsyncTcpServer::StartServer(const ServerOps &ops)
{
	m_isAsynchronousReceive=ops.isAsynchronousReceive;
	return BaseTcpServer::StartServer(ops);
}

void AsyncTcpServer::execute()
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
			AsyncTcpSocket *accWorker=EP_NEW AsyncTcpSocket(m_callBackObj,m_isAsynchronousReceive,m_waitTime,PROCESSOR_LIMIT_INFINITE,m_lockPolicy);
			accWorker->setClientSocket(clientSocket);
			accWorker->setOwner(this);
			accWorker->setSockAddr(sockAddr);
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

