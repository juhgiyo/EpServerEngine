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

AsyncTcpClient::AsyncTcpClient(epl::LockPolicy lockPolicyType) :BaseTcpClient(lockPolicyType)
{
	m_processorList=ServerObjectList(WAITTIME_INIFINITE,lockPolicyType);
	m_maxProcessorCount=PROCESSOR_LIMIT_INFINITE;
	m_isAsynchronousReceive=true;
}


AsyncTcpClient::AsyncTcpClient(const AsyncTcpClient& b) :BaseTcpClient(b)
{
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

		m_processorList=b.m_processorList;
		m_maxProcessorCount=b.m_maxProcessorCount;
		m_isAsynchronousReceive=b.m_isAsynchronousReceive;
	}
	return *this;
}




void AsyncTcpClient::SetMaximumProcessorCount(unsigned int maxProcessorCount)
{
	m_maxProcessorCount=maxProcessorCount;

}
unsigned int AsyncTcpClient::GetMaximumProcessorCount() const
{
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


bool AsyncTcpClient::Connect(const ClientOps &ops)
{
	LockObj lock(m_generalLock);
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
	setSocket(INVALID_SOCKET);
	SOCKET connectSocket = INVALID_SOCKET;
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
		connectSocket = socket(iPtr->ai_family, iPtr->ai_socktype, 
			iPtr->ai_protocol);
		if (connectSocket == INVALID_SOCKET) {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Socket failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
			cleanUpClient();
			return false;
		}

		// Connect to server.
		iResult = connect( connectSocket, iPtr->ai_addr, static_cast<int>(iPtr->ai_addrlen));
		if (iResult == SOCKET_ERROR) {
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	if (connectSocket == INVALID_SOCKET) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Unable to connect to server!\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		cleanUpClient();
		return false;
	}
	setSocket(connectSocket);
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
		cleanUpClient();
		m_processorList.Clear();
		m_callBackObj->OnDisconnect(reinterpret_cast<ClientInterface*>(this));
	}
}

void AsyncTcpClient::Disconnect()
{
	epl::LockObj lock(m_generalLock);
	if(!IsConnectionAlive())
	{
		return;
	}
	SOCKET connectSocket=getSocket();
	if(connectSocket!=INVALID_SOCKET)
	{
		// shutdown the connection since no more data will be sent
		int iResult = shutdown(connectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) shutdown failed with error: %d\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this, WSAGetLastError());
		}

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



