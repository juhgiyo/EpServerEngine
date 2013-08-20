/*! 
IocpTcpSocket for the EpServerEngine

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
#include "epIocpTcpSocket.h"
#include "epIocpTcpServer.h"
#include "epIocpServerJob.h"
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
	IocpServerJob *newJob= EP_NEW IocpServerJob(this,IocpServerJob::IOCP_SERVER_JOB_TYPE_DISCONNECT,NULL,completionEvent,callBackObj,priority,m_lockPolicy);
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
	IocpServerJob *newJob= EP_NEW IocpServerJob(this,IocpServerJob::IOCP_SERVER_JOB_TYPE_SEND,&packet,completionEvent,callBackObj,priority,m_lockPolicy);
	((IocpTcpServer*)m_owner)->pushJob(newJob);
	newJob->ReleaseObj();
}

void IocpTcpSocket::Receive(EventEx *completionEvent,ServerCallbackInterface *callBackObj,Priority priority)
{
	IocpServerJob *newJob= EP_NEW IocpServerJob(this,IocpServerJob::IOCP_SERVER_JOB_TYPE_RECEIVE,NULL,completionEvent,callBackObj,priority,m_lockPolicy);
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


