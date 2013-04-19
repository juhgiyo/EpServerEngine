/*! 
SyncUdpClient for the EpServerEngine
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
#include "epSyncUdpClient.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)


using namespace epse;

SyncUdpClient::SyncUdpClient(epl::LockPolicy lockPolicyType): BaseUdpClient(lockPolicyType)
{
	m_isConnected=false;

}


SyncUdpClient::SyncUdpClient(const SyncUdpClient& b):BaseUdpClient(b)
{
	m_isConnected=false;
}
SyncUdpClient::~SyncUdpClient()
{
}
SyncUdpClient & SyncUdpClient::operator=(const SyncUdpClient&b)
{
	if(this!=&b)
	{				

		BaseUdpClient::operator =(b);
		m_isConnected=false;
	}
	return *this;
}




bool SyncUdpClient::Connect(const ClientOps &ops)
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
	m_isConnected=true;
	return true;
}


bool SyncUdpClient::IsConnectionAlive() const
{
	return m_isConnected;
}

void SyncUdpClient::Disconnect()
{
	epl::LockObj lock(m_generalLock);
	if(!IsConnectionAlive())
	{
		return;
	}
	m_isConnected=false;
	if(m_connectSocket!=INVALID_SOCKET)
	{
		// shutdown the connection since no more data will be sent
		int iResult = shutdown(m_connectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR){
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

void SyncUdpClient::execute()
{}


void SyncUdpClient::disconnect()
{
	if(IsConnectionAlive())
	{
		// No longer need client socket
		m_isConnected=false;
		cleanUpClient();
	
		m_callBackObj->OnDisconnect(this);		
	}

}


int SyncUdpClient::Send(const Packet &packet, unsigned int waitTimeInMilliSec,SendStatus *sendStatus)
{
	epl::LockObj lock(m_generalLock);
	return BaseUdpClient::Send(packet,waitTimeInMilliSec,sendStatus);
}


Packet *SyncUdpClient::Receive(unsigned int waitTimeInMilliSec,ReceiveStatus *retStatus)
{
	epl::LockObj lock(m_generalLock);
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

	FD_ZERO(&fdSet);
	FD_SET(m_connectSocket, &fdSet);
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

	int iResult=0;
	Packet recvPacket(NULL,m_maxPacketSize);
	iResult = receive(recvPacket);

	if (iResult > 0) {
		Packet *passPacket=EP_NEW Packet(recvPacket.GetPacket(),iResult);
		if(retStatus)
			*retStatus=RECEIVE_STATUS_SUCCESS;
		return passPacket;
	}
	else if (iResult == 0)
	{
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Connection closing...\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		disconnect();
		if(retStatus)
			*retStatus=RECEIVE_STATUS_FAIL_CONNECTION_CLOSING;
		return NULL;
	}
	else  {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) recv failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		disconnect();
		if(retStatus)
			*retStatus=RECEIVE_STATUS_FAIL_RECEIVE_FAILED;
		return NULL;
	}
}

