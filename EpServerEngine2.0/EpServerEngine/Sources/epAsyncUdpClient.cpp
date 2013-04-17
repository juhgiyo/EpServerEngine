/*! 
AsyncUdpClient for the EpServerEngine
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
#include "epAsyncUdpClient.h"


#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

AsyncUdpClient::AsyncUdpClient(ClientCallbackInterface *callBackObj,const TCHAR * hostName, const TCHAR * port,bool isAsynchronousReceive,unsigned int waitTimeMilliSec,unsigned int maximumProcessorCount,epl::LockPolicy lockPolicyType): BaseUdpClient(callBackObj,hostName,port,waitTimeMilliSec,lockPolicyType)
{
	m_processorList=ServerObjectList(waitTimeMilliSec,lockPolicyType);
	m_maxProcessorCount=maximumProcessorCount;
	m_isAsynchronousReceive=isAsynchronousReceive;
}

AsyncUdpClient::AsyncUdpClient(const AsyncUdpClient& b):BaseUdpClient(b)
{
	LockObj lock(b.m_generalLock);

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
		LockObj lock(b.m_generalLock);

		m_processorList=b.m_processorList;
		m_maxProcessorCount=b.m_maxProcessorCount;
		m_isAsynchronousReceive=b.m_isAsynchronousReceive;

	}
	return *this;
}


void AsyncUdpClient::SetMaximumProcessorCount(unsigned int maxProcessorCount)
{
	epl::LockObj lock(m_generalLock);
	m_maxProcessorCount=maxProcessorCount;

}
unsigned int AsyncUdpClient::GetMaximumProcessorCount() const
{
	epl::LockObj lock(m_generalLock);
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
				m_callBackObj->OnReceived(reinterpret_cast<ClientInterface*>(this),*passPacket);
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

bool AsyncUdpClient::Connect(const TCHAR * hostName, const TCHAR * port)
{
	epl::LockObj lock(m_generalLock);
	if(IsConnectionAlive())
		return true;

	if(hostName)
	{
		setHostName(hostName);
	}
	if(port)
	{
		setPort(port);
	}

	if(!m_port.length())
	{
		m_port=DEFAULT_PORT;
	}

	if(!m_hostName.length())
	{
		m_hostName=DEFAULT_HOSTNAME;
	}


	WSADATA wsaData;
	m_connectSocket = INVALID_SOCKET;
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
		m_sendLock->Lock();
		if(m_connectSocket!=INVALID_SOCKET)
		{
			closesocket(m_connectSocket);
			m_connectSocket = INVALID_SOCKET;

		}
		m_sendLock->Unlock();

		m_processorList.Clear();
	}
	cleanUpClient();
	m_callBackObj->OnDisconnect(reinterpret_cast<ClientInterface*>(this));
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
		closesocket(m_connectSocket);
		m_connectSocket = INVALID_SOCKET;
	}

	TerminateAfter(m_waitTime);
	
	m_processorList.Clear();
	cleanUpClient();
	m_callBackObj->OnDisconnect(reinterpret_cast<ClientInterface*>(this));

}


