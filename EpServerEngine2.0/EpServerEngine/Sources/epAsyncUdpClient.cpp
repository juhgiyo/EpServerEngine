/*! 
AsyncUdpClient for the EpServerEngine

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
#include "epAsyncUdpClient.h"


#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

AsyncUdpClient::AsyncUdpClient(epl::LockPolicy lockPolicyType): BaseUdpClient(lockPolicyType)
{
	m_processorList=ServerObjectList(WAITTIME_INIFINITE,lockPolicyType);
	m_maxProcessorCount=PROCESSOR_LIMIT_INFINITE;
	m_isAsynchronousReceive=true;
}


AsyncUdpClient::AsyncUdpClient(const AsyncUdpClient& b):BaseUdpClient(b)
{
	m_processorList=b.m_processorList;
	m_maxProcessorCount=b.m_maxProcessorCount;
	m_isAsynchronousReceive=b.m_isAsynchronousReceive;
}
AsyncUdpClient::~AsyncUdpClient()
{
	
}

AsyncUdpClient & AsyncUdpClient::operator=(const AsyncUdpClient&b)
{
	if(this!=&b)
	{				

		BaseUdpClient::operator =(b);
	
		m_processorList=b.m_processorList;
		m_maxProcessorCount=b.m_maxProcessorCount;
		m_isAsynchronousReceive=b.m_isAsynchronousReceive;

	}
	return *this;
}


void AsyncUdpClient::SetMaximumProcessorCount(unsigned int maxProcessorCount)
{
	m_maxProcessorCount=maxProcessorCount;

}
unsigned int AsyncUdpClient::GetMaximumProcessorCount() const
{
	return m_maxProcessorCount;
}



void AsyncUdpClient::SetWaitTime(unsigned int milliSec)
{
	m_waitTime=milliSec;
	m_processorList.SetWaitTime(milliSec);
}

bool AsyncUdpClient::GetIsAsynchronousReceive() const
{
	return m_isAsynchronousReceive;
}
void AsyncUdpClient::SetIsAsynchronousReceive(bool isASynchronousReceive)
{
	m_isAsynchronousReceive=isASynchronousReceive;
}

void AsyncUdpClient::execute() 
{
	int iResult=0;
	// Receive until the peer shuts down the connection
	Packet recvPacket(NULL,m_maxPacketSize);
	do {
		iResult = receive(recvPacket);

		if (iResult > 0) {
			Packet *passPacket=EP_NEW Packet(recvPacket.GetPacket(),iResult);
			if(m_isAsynchronousReceive)
			{
				ClientPacketProcessor::PacketPassUnit passUnit;

				passUnit.m_packet=passPacket;
				passUnit.m_owner=this;
				ClientPacketProcessor *parser=EP_NEW ClientPacketProcessor(m_callBackObj,m_waitTime,m_lockPolicy);
				parser->setPacketPassUnit(passUnit);
				m_processorList.Push(parser);
				parser->Start();
				parser->ReleaseObj();
				passPacket->ReleaseObj();
				unsigned int maximumProcessorCount=GetMaximumProcessorCount();
				if(maximumProcessorCount!=PROCESSOR_LIMIT_INFINITE)
				{
					while(m_processorList.Count()>=maximumProcessorCount)
					{
						m_processorList.WaitForListSizeDecrease();
					}
				}
			}
			else
			{
				m_callBackObj->OnReceived(reinterpret_cast<ClientInterface*>(this),passPacket,RECEIVE_STATUS_SUCCESS);
				passPacket->ReleaseObj();
			}

		}
		else if (iResult == 0)
		{
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Connection closing...\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
			break;
		}
		else  {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) recv failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
			break;
		}

	} while (iResult > 0);

	disconnect();
}

bool AsyncUdpClient::Connect(const ClientOps &ops)
{
	epl::LockObj lock(m_generalLock);
	if(IsConnectionAlive())
		return true;

	if(ops.callBackObj)
		m_callBackObj=ops.callBackObj;
	EP_ASSERT(m_callBackObj);

	if(ops.hostName)
	{
		setHostName(ops.hostName);
	}
	if(ops.port)
	{
		setPort(ops.port);
	}

	if(!m_port.length())
	{
		m_port=DEFAULT_PORT;
	}

	if(!m_hostName.length())
	{
		m_hostName=DEFAULT_HOSTNAME;
	}
	SetWaitTime(ops.waitTimeMilliSec);
	m_maxProcessorCount=ops.maximumProcessorCount;
	m_isAsynchronousReceive=ops.isAsynchronousReceive;


	WSADATA wsaData;
	m_connectSocket=INVALID_SOCKET;
	m_maxPacketSize=0;
	struct addrinfo hints;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) WSAStartup failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		return false;
	}

	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	// Resolve the server address and port
	iResult = getaddrinfo(m_hostName.c_str(), m_port.c_str(), &hints, &m_result);
	if ( iResult != 0 ) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) getaddrinfo failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		WSACleanup();
		return false;
	}

	// Attempt to connect to an address until one succeeds
	for(m_ptr=m_result; m_ptr != NULL ;m_ptr=m_ptr->ai_next) {

		// Create a SOCKET for connecting to server
		m_connectSocket = socket(m_ptr->ai_family, m_ptr->ai_socktype, 
			m_ptr->ai_protocol);
		if (m_connectSocket == INVALID_SOCKET) {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Socket failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
			cleanUpClient();
			return false;
		}
		break;
	}
	if (m_connectSocket == INVALID_SOCKET) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Unable to connect to server!\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		cleanUpClient();
		return false;
	}

	int nTmp = sizeof(int);
	getsockopt(m_connectSocket, SOL_SOCKET,SO_MAX_MSG_SIZE, (char *)&m_maxPacketSize,&nTmp);

	if(Start())
	{
		return true;
	}
	cleanUpClient();
	return false;
}


void AsyncUdpClient::disconnect()
{
	if(IsConnectionAlive())
	{
		cleanUpClient();
		m_processorList.Clear();
		m_callBackObj->OnDisconnect(reinterpret_cast<ClientInterface*>(this));
	}
}

void AsyncUdpClient::Disconnect()
{
	epl::LockObj lock(m_generalLock);
	if(!IsConnectionAlive())
	{
		return;
	}
	if(m_connectSocket!=INVALID_SOCKET)
	{
		int iResult = shutdown(m_connectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR)
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) shutdown failed with error: %d\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this, WSAGetLastError());
	}
	else
	{
		return;
	}
	if(TerminateAfter(m_waitTime)==Thread::TERMINATE_RESULT_GRACEFULLY_TERMINATED)
		return;
	cleanUpClient();
	m_processorList.Clear();
	m_callBackObj->OnDisconnect(reinterpret_cast<ClientInterface*>(this));

}


