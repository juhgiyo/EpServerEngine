/*! 
BaseTcpSocket for the EpServerEngine

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
#include "epBaseTcpSocket.h"
#include "epBaseTcpServer.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;
BaseTcpSocket::BaseTcpSocket(ServerCallbackInterface *callBackObj,unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType): BaseSocket(callBackObj,waitTimeMilliSec,lockPolicyType)
{
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_sendLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_sendLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_sendLock=EP_NEW epl::NoLock();
		break;
	default:
		m_sendLock=NULL;
		break;
	}
	m_recvSizePacket=Packet(NULL,4);
	m_clientSocket=INVALID_SOCKET;
}

BaseTcpSocket::~BaseTcpSocket()
{
	if(m_sendLock)
		EP_DELETE m_sendLock;
	m_sendLock=NULL;
}

void BaseTcpSocket::setClientSocket(const SOCKET& clientSocket )
{
	epl::LockObj lock(m_sendLock);
	m_clientSocket=clientSocket;
}


int BaseTcpSocket::Send(const Packet &packet, unsigned int waitTimeInMilliSec,SendStatus *sendStatus)
{	
	epl::LockObj lock(m_sendLock);

	if(m_clientSocket==INVALID_SOCKET)
	{
		if(sendStatus)
			*sendStatus=SEND_STATUS_FAIL_NOT_CONNECTED;
		return 0;
	}

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
		retfdNum = select(0, NULL, &fdSet, NULL, &timeOutVal);
	}
	else
	{
		retfdNum = select(0, NULL, &fdSet, NULL, NULL);
	}
	if (retfdNum == SOCKET_ERROR)	// select failed
	{
		if(sendStatus)
			*sendStatus=SEND_STATUS_FAIL_SOCKET_ERROR;
		return retfdNum;
	}
	else if (retfdNum == 0)		// select time-out
	{
		if(sendStatus)
			*sendStatus=SEND_STATUS_FAIL_TIME_OUT;
		return retfdNum;
	}

	// send routine
	int writeLength=0;
	const char *packetData=packet.GetPacket();
	int length=packet.GetPacketByteSize();

	if(length>0)
	{
		int sentLength=send(m_clientSocket,reinterpret_cast<char*>(&length),4,0);
		if(sentLength<=0)
		{
			if(sendStatus)
				*sendStatus=SEND_STATUS_SUCCESS;
			return sentLength;
		}
	}
	while(length>0)
	{
		int sentLength=send(m_clientSocket,packetData,length,0);
		writeLength+=sentLength;
		if(sentLength<=0)
		{
			if(sendStatus)
				*sendStatus=SEND_STATUS_FAIL_SEND_FAILED;
			return sentLength;
		}
		length-=sentLength;
		packetData+=sentLength;
	}
	if(sendStatus)
		*sendStatus=SEND_STATUS_SUCCESS;
	return writeLength;
}


int BaseTcpSocket::receive(Packet &packet)
{
	int readLength=0;
	int length=packet.GetPacketByteSize();
	char *packetData=const_cast<char*>(packet.GetPacket());
	while(length>0)
	{
		int recvLength=recv(m_clientSocket,packetData, length, 0);
		readLength+=recvLength;
		if(recvLength<=0)
		{
			return recvLength;
		}
		length-=recvLength;
		packetData+=recvLength;
	}
	return readLength;
}



