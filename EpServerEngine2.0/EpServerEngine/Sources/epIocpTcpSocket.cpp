/*! 
IocpTcpSocket for the EpServerEngine
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
#include "epIocpTcpSocket.h"
#include "epIocpTcpServer.h"
#include "epIocpTcpJob.h"
#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;
IocpTcpSocket::IocpTcpSocket(ServerCallbackInterface *callBackObj,unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType): BaseTcpSocket(callBackObj,waitTimeMilliSec,lockPolicyType)
{
	m_isConnected=true;
}

IocpTcpSocket::~IocpTcpSocket()
{
	killConnection();
}

bool IocpTcpSocket::IsConnectionAlive() const
{
	return m_isConnected;
}

void IocpTcpSocket::KillConnection(EventEx *completionEvent,ServerCallbackInterface *callBackObj,Priority priority)
{
	IocpTcpJob *newJob= EP_NEW IocpTcpJob(this,IocpTcpJob::IOCP_TCP_JOB_TYPE_DISCONNECT,NULL,completionEvent,callBackObj,priority,m_lockPolicy);
	((IocpTcpServer*)m_owner)->pushJob(newJob);
	newJob->ReleaseObj();
}
void IocpTcpSocket::KillConnection()
{
	KillConnection(NULL,NULL,PRIORITY_NORMAL);
}

void IocpTcpSocket::killConnection()
{
	if(IsConnectionAlive())
	{
		m_isConnected=false;
		// No longer need client socket
		if(m_clientSocket!=INVALID_SOCKET)
		{
			closesocket(m_clientSocket);
			m_clientSocket = INVALID_SOCKET;
		}


		removeSelfFromContainer();
		m_callBackObj->OnDisconnect(this);
	}
}

void IocpTcpSocket::killConnectionNoCallBack()
{
	if(IsConnectionAlive())
	{
		m_isConnected=false;
		// No longer need client socket
		if(m_clientSocket!=INVALID_SOCKET)
		{
			closesocket(m_clientSocket);
			m_clientSocket = INVALID_SOCKET;
		}


		removeSelfFromContainer();
	}
}

void IocpTcpSocket::Send(Packet &packet,EventEx *completionEvent,ServerCallbackInterface *callBackObj,Priority priority)
{
	IocpTcpJob *newJob= EP_NEW IocpTcpJob(this,IocpTcpJob::IOCP_TCP_JOB_TYPE_SEND,&packet,completionEvent,callBackObj,priority,m_lockPolicy);
	((IocpTcpServer*)m_owner)->pushJob(newJob);
	newJob->ReleaseObj();
}

void IocpTcpSocket::Receive(EventEx *completionEvent,ServerCallbackInterface *callBackObj,Priority priority)
{
	IocpTcpJob *newJob= EP_NEW IocpTcpJob(this,IocpTcpJob::IOCP_TCP_JOB_TYPE_RECEIVE,NULL,completionEvent,callBackObj,priority,m_lockPolicy);
	((IocpTcpServer*)m_owner)->pushJob(newJob);
	newJob->ReleaseObj();
}
int IocpTcpSocket::Send(const Packet &packet, unsigned int waitTimeInMilliSec,SendStatus *sendStatus)
{
	epl::LockObj lock(m_baseSocketLock);
	return BaseTcpSocket::Send(packet,waitTimeInMilliSec,sendStatus);
}

Packet *IocpTcpSocket::Receive(unsigned int waitTimeInMilliSec,ReceiveStatus *retStatus)
{
	epl::LockObj lock(m_baseSocketLock);
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
	FD_SET(m_clientSocket, &fdSet);
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
		killConnection();
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
	int iResult =receive(m_recvSizePacket);
	if(iResult>0)
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
			killConnection();
			if(retStatus)
				*retStatus=RECEIVE_STATUS_FAIL_CONNECTION_CLOSING;
			return NULL;
		}
		else  {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) recv failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
			recvPacket->ReleaseObj();
			killConnection();
			if(retStatus)
				*retStatus=RECEIVE_STATUS_FAIL_RECEIVE_FAILED;
			return NULL;
		}
	}
	else
	{
		killConnection();
		if(retStatus)
			*retStatus=RECEIVE_STATUS_FAIL_CONNECTION_CLOSING;
		return NULL;
	}

}

void IocpTcpSocket::execute()
{
	m_callBackObj->OnNewConnection(this);
}


