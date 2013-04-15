/*! 
AsyncTcpSocket for the EpServerEngine
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
#include "epAsyncTcpSocket.h"
#include "epAsyncTcpServer.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;
AsyncTcpSocket::AsyncTcpSocket(ServerCallbackInterface *callBackObj,bool isAsynchronousReceive,unsigned int waitTimeMilliSec,unsigned int maximumProcessorCount,epl::LockPolicy lockPolicyType): BaseTcpSocket(callBackObj,waitTimeMilliSec,lockPolicyType)
{
	m_maxProcessorCount=maximumProcessorCount;
	m_isAsynchronousReceive=isAsynchronousReceive;
}

AsyncTcpSocket::~AsyncTcpSocket()
{
	KillConnection();
}




void AsyncTcpSocket::SetMaximumProcessorCount(unsigned int maxParserCount)
{
	epl::LockObj lock(m_baseSocketLock);
	m_maxProcessorCount=maxParserCount;

}
unsigned int AsyncTcpSocket::GetMaximumProcessorCount() const
{
	epl::LockObj lock(m_baseSocketLock);
	return m_maxProcessorCount;
}
bool AsyncTcpSocket::GetIsAsynchronousReceive() const
{
	return m_isAsynchronousReceive;
}
void AsyncTcpSocket::SetIsAsynchronousReceive(bool isASynchronousReceive)
{
	m_isAsynchronousReceive=isASynchronousReceive;
}

void AsyncTcpSocket::KillConnection()
{
	epl::LockObj lock(m_baseSocketLock);
	if(!IsConnectionAlive())
	{
		return;
	}
	// No longer need client socket
	m_sendLock->Lock();
	if(m_clientSocket!=INVALID_SOCKET)
	{
		int iResult;
		iResult = shutdown(m_clientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) shutdown failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		}
		closesocket(m_clientSocket);
		m_clientSocket = INVALID_SOCKET;
	}
	m_sendLock->Unlock();

	TerminateAfter(m_waitTime);
	m_processorList.Clear();

	removeSelfFromContainer();
	m_callBackObj->OnDisconnect(this);
}


void AsyncTcpSocket::killConnection()
{
	if(IsConnectionAlive())
	{
		// No longer need client socket
		m_sendLock->Lock();
		if(m_clientSocket!=INVALID_SOCKET)
		{
			closesocket(m_clientSocket);
			m_clientSocket = INVALID_SOCKET;
		}
		m_sendLock->Unlock();
		m_processorList.Clear();
		removeSelfFromContainer();
		m_callBackObj->OnDisconnect(this);
	}
}

void AsyncTcpSocket::execute()
{
	int iResult=0;

	

	// Receive until the peer shuts down the connection
	do {
		iResult =receive(m_recvSizePacket);
		if(iResult>0)
		{
			unsigned int shouldReceive=(reinterpret_cast<unsigned int*>(const_cast<char*>(m_recvSizePacket.GetPacket())))[0];
			Packet *recvPacket=EP_NEW Packet(NULL,shouldReceive);
			iResult = receive(*recvPacket);

			if (iResult == shouldReceive) {
				if(m_isAsynchronousReceive)
				{
					ServerPacketProcessor::PacketPassUnit passUnit;
					passUnit.m_packet=recvPacket;
					passUnit.m_owner=this;
					ServerPacketProcessor *parser =EP_NEW ServerPacketProcessor(m_callBackObj,m_waitTime,m_lockPolicy);
					parser->setPacketPassUnit(passUnit);
					m_processorList.Push(parser);
					parser->Start();
					parser->ReleaseObj();
					recvPacket->ReleaseObj();
					if(GetMaximumProcessorCount()!=PROCESSOR_LIMIT_INFINITE)
					{
						while(m_processorList.Count()>=GetMaximumProcessorCount())
						{
							m_processorList.WaitForListSizeDecrease();
						}
					}
				}
				else
				{
					m_callBackObj->OnReceived(this,&recvPacket);
					recvPacket->ReleaseObj();
				}
				
			}
			else if (iResult == 0)
			{
				epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Connection closing...\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
				recvPacket->ReleaseObj();
				break;
			}
			else  {
				epl::System::OutputDebugString(_T("%s::%s(%d)(%x) recv failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
				recvPacket->ReleaseObj();
				break;
			}
		}
		else
		{
			break;
		}

	} while (iResult > 0);

	killConnection();
}


