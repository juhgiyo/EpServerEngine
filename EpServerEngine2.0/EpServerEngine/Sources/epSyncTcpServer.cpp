/*! 
BaseServer for the SyncTcpServer

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
#include "epSyncTcpServer.h"
#include "epSyncTcpSocket.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

SyncTcpServer::SyncTcpServer(epl::LockPolicy lockPolicyType):BaseTcpServer(lockPolicyType)
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
			if(!accWorker)
			{
				closesocket(clientSocket);
				continue;
			}
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
