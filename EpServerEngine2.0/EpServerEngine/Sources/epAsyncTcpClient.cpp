/*! 
AsyncTcpClient for the EpServerEngine
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
#include "epAsyncTcpClient.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

AsyncTcpClient::AsyncTcpClient(ClientCallbackInterface *callBackObj,const TCHAR * hostName, const TCHAR * port,bool isAsynchronousReceive,unsigned int waitTimeMilliSec,unsigned int maximumProcessorCount,epl::LockPolicy lockPolicyType) :BaseTcpClient(callBackObj,hostName,port,waitTimeMilliSec,lockPolicyType)
{
	m_processorList=ServerObjectList(waitTimeMilliSec,lockPolicyType);
	m_maxProcessorCount=maximumProcessorCount;
	m_isAsynchronousReceive=isAsynchronousReceive;
}
AsyncTcpClient::AsyncTcpClient(const ClientOps &ops):BaseTcpClient(ops)
{
	m_processorList=ServerObjectList(ops.waitTimeMilliSec,ops.lockPolicyType);
	m_maxProcessorCount=ops.maximumProcessorCount;
	m_isAsynchronousReceive=ops.isAsynchronousReceive;
}

AsyncTcpClient::AsyncTcpClient(const AsyncTcpClient& b) :BaseTcpClient(b)
{
	LockObj lock(b.m_generalLock);
	
	m_processorList=b.m_processorList;
	m_maxProcessorCount=b.m_maxProcessorCount;
	m_isAsynchronousReceive=b.m_isAsynchronousReceive;
	
}
AsyncTcpClient::~AsyncTcpClient()
{
}

AsyncTcpClient & AsyncTcpClient::operator=(const AsyncTcpClient&b)
{
	if(this!=&b)
	{

		BaseTcpClient::operator =(b);

		LockObj lock(b.m_generalLock);
		m_processorList=b.m_processorList;
		m_maxProcessorCount=b.m_maxProcessorCount;
		m_isAsynchronousReceive=b.m_isAsynchronousReceive;
	}
	return *this;
}




void AsyncTcpClient::SetMaximumProcessorCount(unsigned int maxProcessorCount)
{
	epl::LockObj lock(m_generalLock);
	m_maxProcessorCount=maxProcessorCount;

}
unsigned int AsyncTcpClient::GetMaximumProcessorCount() const
{
	epl::LockObj lock(m_generalLock);
	return m_maxProcessorCount;
}

void AsyncTcpClient::SetWaitTime(unsigned int milliSec)
{
	m_waitTime=milliSec;
	m_processorList.SetWaitTime(milliSec);
}

bool AsyncTcpClient::GetIsAsynchronousReceive() const
{
	return m_isAsynchronousReceive;
}
void AsyncTcpClient::SetIsAsynchronousReceive(bool isASynchronousReceive)
{
	m_isAsynchronousReceive=isASynchronousReceive;
}

void AsyncTcpClient::execute() 
{
	int iResult;
	// Receive until the peer shuts down the connection
	do {
		int size =receive(m_recvSizePacket);
		if(size>0)
		{
			unsigned int shouldReceive=(reinterpret_cast<unsigned int*>(const_cast<char*>(m_recvSizePacket.GetPacket())))[0];
			Packet *recvPacket=EP_NEW Packet(NULL,shouldReceive);
			iResult = receive(*recvPacket);

			if (iResult == shouldReceive) {
				if(m_isAsynchronousReceive)
				{
					ClientPacketProcessor::PacketPassUnit passUnit;
					passUnit.m_packet=recvPacket;
					passUnit.m_owner=this;
					ClientPacketProcessor *parser =EP_NEW ClientPacketProcessor(m_callBackObj,m_waitTime,m_lockPolicy);
					EP_ASSERT(parser);
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
					m_callBackObj->OnReceived(reinterpret_cast<ClientInterface*>(this),*recvPacket);
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

	disconnect();
}


bool AsyncTcpClient::Connect(const TCHAR * hostName, const TCHAR * port)
{
	LockObj lock(m_generalLock);
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
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(m_hostName.c_str(), m_port.c_str(), &hints, &m_result);
	if ( iResult != 0 ) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) getaddrinfo failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		WSACleanup();
		return false;
	}

	struct addrinfo *iPtr=0;
	// Attempt to connect to an address until one succeeds
	for(iPtr=m_result; iPtr != NULL ;iPtr=iPtr->ai_next) {

		// Create a SOCKET for connecting to server
		m_connectSocket = socket(iPtr->ai_family, iPtr->ai_socktype, 
			iPtr->ai_protocol);
		if (m_connectSocket == INVALID_SOCKET) {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Socket failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
			cleanUpClient();
			return false;
		}

		// Connect to server.
		iResult = connect( m_connectSocket, iPtr->ai_addr, static_cast<int>(iPtr->ai_addrlen));
		if (iResult == SOCKET_ERROR) {
			closesocket(m_connectSocket);
			m_connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	if (m_connectSocket == INVALID_SOCKET) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Unable to connect to server!\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		cleanUpClient();
		return false;
	}
	if(Start())
	{
		return true;
	}
	cleanUpClient();
	return false;
}


void AsyncTcpClient::disconnect()
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

void AsyncTcpClient::Disconnect()
{
	epl::LockObj lock(m_generalLock);
	if(!IsConnectionAlive())
	{
		return;
	}
	if(m_connectSocket!=INVALID_SOCKET)
	{
		// shutdown the connection since no more data will be sent
		int iResult = shutdown(m_connectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) shutdown failed with error: %d\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this, WSAGetLastError());
		}
		closesocket(m_connectSocket);
		m_connectSocket = INVALID_SOCKET;

	}
	TerminateAfter(m_waitTime);

	m_processorList.Clear();
	cleanUpClient();
	m_callBackObj->OnDisconnect(reinterpret_cast<ClientInterface*>(this));

}



