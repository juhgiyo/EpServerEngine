/*! 
AsyncTcpSocket for the EpServerEngine

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
	m_processorList=ServerObjectList(waitTimeMilliSec,lockPolicyType);
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

void AsyncTcpSocket::SetWaitTime(unsigned int milliSec)
{
	m_waitTime=milliSec;
	m_processorList.SetWaitTime(milliSec);
}

void AsyncTcpSocket::KillConnection()
{
	epl::LockObj lock(m_baseSocketLock);
	if(!IsConnectionAlive())
	{
		return;
	}
	// No longer need client socket
	if(m_clientSocket!=INVALID_SOCKET)
	{
		int iResult;
		iResult = shutdown(m_clientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) shutdown failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		}
	}
	else
	{
		return;
	}

	if(TerminateAfter(m_waitTime)==Thread::TERMINATE_RESULT_GRACEFULLY_TERMINATED)
		return;

	if(m_clientSocket!=INVALID_SOCKET)
	{
		closesocket(m_clientSocket);
		m_clientSocket = INVALID_SOCKET;
	}

	m_processorList.Clear();
	removeSelfFromContainer();
	m_callBackObj->OnDisconnect(this);
}


void AsyncTcpSocket::killConnection()
{
	if(IsConnectionAlive())
	{
		// No longer need client socket
		if(m_clientSocket!=INVALID_SOCKET)
		{
			closesocket(m_clientSocket);
			m_clientSocket = INVALID_SOCKET;
		}
		m_processorList.Clear();
		removeSelfFromContainer();
		m_callBackObj->OnDisconnect(this);
	}
}

void AsyncTcpSocket::execute()
{
	m_callBackObj->OnNewConnection(this);

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
					if(!parser)
					{
						recvPacket->ReleaseObj();
						continue;
					}
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
					m_callBackObj->OnReceived(this,recvPacket,RECEIVE_STATUS_SUCCESS);
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


