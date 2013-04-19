/*! 
SyncTcpClient for the EpServerEngine
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
#include "epSyncTcpClient.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

SyncTcpClient::SyncTcpClient(epl::LockPolicy lockPolicyType) :BaseTcpClient(lockPolicyType)
{
	m_isConnected=false;
}


SyncTcpClient::SyncTcpClient(const SyncTcpClient& b) :BaseTcpClient(b)
{
	m_isConnected=false;
}

SyncTcpClient::~SyncTcpClient()
{
}

SyncTcpClient & SyncTcpClient::operator=(const SyncTcpClient&b)
{
	if(this!=&b)
	{

		BaseTcpClient::operator =(b);
		
		m_isConnected=false;
	}
	return *this;
}


void SyncTcpClient::execute()
{}

bool SyncTcpClient::Connect(const ClientOps &ops)
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

	// Attempt to connect to an address until one succeeds
	struct addrinfo *iPtr=0;
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
	m_isConnected=true;
	return true;

}


bool SyncTcpClient::IsConnectionAlive() const
{
	return m_isConnected;
}

void SyncTcpClient::Disconnect()
{
	epl::LockObj lock(m_generalLock);
	if(!IsConnectionAlive())
	{
		return;
	}
	m_isConnected=false;	
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

	cleanUpClient();
	m_callBackObj->OnDisconnect(this);
}

void SyncTcpClient::disconnect()
{
	if(IsConnectionAlive())
	{
		// No longer need client socket
		m_isConnected=false;	
		cleanUpClient();
		m_callBackObj->OnDisconnect(this);
	}


	
}





Packet *SyncTcpClient::Receive(unsigned int waitTimeInMilliSec,ReceiveStatus *retStatus)
{
	if(!IsConnectionAlive())
	{
		if(retStatus)
			*retStatus=RECEIVE_STATUS_FAIL_NOT_CONNECTED;
		return NULL;
	}

	// select routine
	TIMEVAL	timeOutVal;
	fd_set	fdSet;
	int		retfdNum = 0;
	SOCKET connectSocket=getSocket();

	FD_ZERO(&fdSet);
	FD_SET(connectSocket, &fdSet);
	if(waitTimeInMilliSec!=WAITTIME_INIFINITE)
	{
		// socket select time out setting
		timeOutVal.tv_sec = (long)(waitTimeInMilliSec/1000); // Convert to seconds
		timeOutVal.tv_usec = (long)(waitTimeInMilliSec%1000)*1000; // Convert remainders to micro-seconds
		// socket select
		// socket read select
		retfdNum = select(0,&fdSet, NULL, NULL, &timeOutVal);
	}
	else
	{
		retfdNum = select(0, &fdSet,NULL, NULL, NULL);
	}
	if (retfdNum == SOCKET_ERROR)	// select failed
	{
		disconnect();
		if(retStatus)
			*retStatus=RECEIVE_STATUS_FAIL_SOCKET_ERROR;
		return NULL;
	}
	else if (retfdNum == 0)		    // select time-out
	{
		if(retStatus)
			*retStatus=RECEIVE_STATUS_FAIL_TIME_OUT;
		return NULL;
	}

	// receive routine
	int iResult;
	int size =receive(m_recvSizePacket);
	if(size>0)
	{
		unsigned int shouldReceive=(reinterpret_cast<unsigned int*>(const_cast<char*>(m_recvSizePacket.GetPacket())))[0];
		Packet *recvPacket=EP_NEW Packet(NULL,shouldReceive);
		iResult = receive(*recvPacket);

		if (iResult == shouldReceive) {
			if(retStatus)
				*retStatus=RECEIVE_STATUS_SUCCESS;
			return recvPacket;
		}
		else if (iResult == 0)
		{
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Connection closing...\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
			recvPacket->ReleaseObj();
			disconnect();
			if(retStatus)
				*retStatus=RECEIVE_STATUS_FAIL_CONNECTION_CLOSING;
			return NULL;
		}
		else  {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) recv failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
			recvPacket->ReleaseObj();
			disconnect();
			if(retStatus)
				*retStatus=RECEIVE_STATUS_FAIL_RECEIVE_FAILED;
			return NULL;
		}
	}
	else
	{
		disconnect();
		return NULL;
	}
}
