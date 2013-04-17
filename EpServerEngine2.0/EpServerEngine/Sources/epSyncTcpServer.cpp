/*! 
BaseServer for the SyncTcpServer
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
#include "epSyncTcpServer.h"
#include "epSyncTcpSocket.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

SyncTcpServer::SyncTcpServer(ServerCallbackInterface *callBackObj,const TCHAR * port,unsigned int waitTimeMilliSec, unsigned int maximumConnectionCount, epl::LockPolicy lockPolicyType):BaseTcpServer(callBackObj,port,waitTimeMilliSec,maximumConnectionCount,lockPolicyType)
{
}
SyncTcpServer::SyncTcpServer(const ServerOps &ops):BaseTcpServer(ops)
{

}

SyncTcpServer::SyncTcpServer(const SyncTcpServer& b):BaseTcpServer(b)
{
}
SyncTcpServer::~SyncTcpServer()
{
}

SyncTcpServer & SyncTcpServer::operator=(const SyncTcpServer&b)
{
	if(this!=&b)
	{
		BaseTcpServer::operator =(b);
	}
	return *this;
}

void SyncTcpServer::execute()
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
			SyncTcpSocket *accWorker=EP_NEW SyncTcpSocket(m_callBackObj,m_waitTime,m_lockPolicy);
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
