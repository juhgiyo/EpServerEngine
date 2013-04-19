/*! 
BaseUdpClient for the EpServerEngine
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
		return 0;

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



