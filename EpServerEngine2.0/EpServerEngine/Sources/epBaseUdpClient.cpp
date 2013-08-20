/*! 
BaseUdpClient for the EpServerEngine

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
#include "epBaseUdpClient.h"


#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

BaseUdpClient::BaseUdpClient(epl::LockPolicy lockPolicyType): BaseClient(lockPolicyType)
{

	m_ptr=0;
	m_maxPacketSize=0;
}

BaseUdpClient::BaseUdpClient(const BaseUdpClient& b):BaseClient(b)
{
	m_ptr=0;
	m_maxPacketSize=b.m_maxPacketSize;


}
BaseUdpClient::~BaseUdpClient()
{

}

BaseUdpClient & BaseUdpClient::operator=(const BaseUdpClient&b)
{
	if(this!=&b)
	{				

		BaseClient::operator =(b);

		m_ptr=0;
		m_maxPacketSize=b.m_maxPacketSize;


	}
	return *this;
}

unsigned int BaseUdpClient::GetMaxPacketByteSize() const
{
	return m_maxPacketSize;
}

int BaseUdpClient::Send(const Packet &packet, unsigned int waitTimeInMilliSec,SendStatus *sendStatus)
{
	epl::LockObj lock(m_sendLock);
	if(!IsConnectionAlive())
	{
		if(sendStatus)
			*sendStatus=SEND_STATUS_FAIL_NOT_CONNECTED;
		return 0;
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
	else if (retfdNum == 0)		    // select time-out
	{
		if(sendStatus)
			*sendStatus=SEND_STATUS_FAIL_TIME_OUT;
		return retfdNum;
	}

	// send routine
	int sentLength=0;
	int writeLength=0;
	const char *packetData=packet.GetPacket();
	int length=packet.GetPacketByteSize();
	EP_ASSERT(length<=m_maxPacketSize);
	int sockAddrSize=sizeof(sockaddr);
	while(length>0)
	{
		sentLength=sendto(m_connectSocket,packetData,length,0,m_ptr->ai_addr,sizeof(sockaddr));
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
	return sentLength;
}



int BaseUdpClient::receive(Packet &packet)
{
	int length=packet.GetPacketByteSize();
	char *packetData=const_cast<char*>(packet.GetPacket());
	sockaddr tmpInfo;
	int tmpInfoSize=sizeof(sockaddr);
	int recvLength = recvfrom(m_connectSocket,packetData,length,0,&tmpInfo,&tmpInfoSize);
	return recvLength;
}


void BaseUdpClient::cleanUpClient()
{
	BaseClient::cleanUpClient();
	m_maxPacketSize=0;
}



