/*! 
SyncUdpServer for the EpServerEngine

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
#include "epSyncUdpServer.h"
#include "epSyncUdpSocket.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

SyncUdpServer::SyncUdpServer(epl::LockPolicy lockPolicyType): BaseUdpServer(lockPolicyType)
{
}


SyncUdpServer::SyncUdpServer(const SyncUdpServer& b):BaseUdpServer(b)
{
}
SyncUdpServer::~SyncUdpServer()
{
}
SyncUdpServer & SyncUdpServer::operator=(const SyncUdpServer&b)
{
	if(this!=&b)
	{
		BaseUdpServer::operator =(b);
	}
	return *this;
}

void SyncUdpServer::execute()
{
	Packet recvPacket(NULL,m_maxPacketSize);
	char *packetData=const_cast<char*>(recvPacket.GetPacket());
	int length=recvPacket.GetPacketByteSize();
	sockaddr clientSockAddr;
	int sockAddrSize=sizeof(sockaddr);
	while(m_listenSocket!=INVALID_SOCKET)
	{
		int recvLength=recvfrom(m_listenSocket,packetData,length, 0,&clientSockAddr,&sockAddrSize);

		SyncUdpSocket *workerObj=(SyncUdpSocket*)m_socketList.Find(clientSockAddr,socketCompare);
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
			SyncUdpSocket *accWorker=EP_NEW SyncUdpSocket(m_callBackObj,m_waitTime,m_lockPolicy);
			if(!accWorker)
			{
				continue;
			}
			Packet *passPacket=EP_NEW Packet(packetData,recvLength);
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
