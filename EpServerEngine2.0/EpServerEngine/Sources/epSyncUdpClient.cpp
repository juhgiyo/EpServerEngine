/*! 
SyncUdpClient for the EpServerEngine

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

