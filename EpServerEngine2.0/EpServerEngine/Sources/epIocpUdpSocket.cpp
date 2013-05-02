/*! 
IocpUdpSocket for the EpServerEngine
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
#include "epIocpUdpSocket.h"
#include "epIocpUdpServer.h"
#include "epIocpServerJob.h"
#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;
IocpUdpSocket::IocpUdpSocket(ServerCallbackInterface *callBackObj,unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType): BaseUdpSocket(callBackObj,waitTimeMilliSec,lockPolicyType)
{
	m_packetReceivedEvent=EventEx(false,false);
	m_isConnected=true;
}

IocpUdpSocket::~IocpUdpSocket()
{
	killConnection();
}

bool IocpUdpSocket::IsConnectionAlive() const
{
	return m_isConnected;
}

void IocpUdpSocket::KillConnection(EventEx *completionEvent,ServerCallbackInterface *callBackObj,Priority priority)
{
	IocpServerJob *newJob= EP_NEW IocpServerJob(this,IocpServerJob::IOCP_SERVER_JOB_TYPE_DISCONNECT,NULL,completionEvent,callBackObj,priority,m_lockPolicy);
	((IocpUdpServer*)m_owner)->pushJob(newJob);
	newJob->ReleaseObj();
}
void IocpUdpSocket::KillConnection()
{
	KillConnection(NULL,NULL,PRIORITY_NORMAL);
}

void IocpUdpSocket::killConnection()
{
	if(IsConnectionAlive())
	{
		m_isConnected=false;

		m_listLock->Lock();
		Packet *removeElem=NULL;
		while(!m_packetList.empty())
		{
			removeElem=m_packetList.front();
			if(removeElem)
				removeElem->ReleaseObj();
			m_packetList.pop();
		}
		m_listLock->Unlock();


		removeSelfFromContainer();
		m_callBackObj->OnDisconnect(this);

	}
}

void IocpUdpSocket::killConnectionNoCallBack()
{
	if(IsConnectionAlive())
	{
		m_isConnected=false;

		m_listLock->Lock();
		Packet *removeElem=NULL;
		while(!m_packetList.empty())
		{
			removeElem=m_packetList.front();
			if(removeElem)
				removeElem->ReleaseObj();
			m_packetList.pop();
		}
		m_listLock->Unlock();


		removeSelfFromContainer();

	}
}

void IocpUdpSocket::Send(Packet &packet,EventEx *completionEvent,ServerCallbackInterface *callBackObj,Priority priority)
{
	IocpServerJob *newJob= EP_NEW IocpServerJob(this,IocpServerJob::IOCP_SERVER_JOB_TYPE_SEND,&packet,completionEvent,callBackObj,priority,m_lockPolicy);
	((IocpUdpServer*)m_owner)->pushJob(newJob);
	newJob->ReleaseObj();
}

void IocpUdpSocket::Receive(EventEx *completionEvent,ServerCallbackInterface *callBackObj,Priority priority)
{
	IocpServerJob *newJob= EP_NEW IocpServerJob(this,IocpServerJob::IOCP_SERVER_JOB_TYPE_RECEIVE,NULL,completionEvent,callBackObj,priority,m_lockPolicy);
	((IocpUdpServer*)m_owner)->pushJob(newJob);
	newJob->ReleaseObj();
}
int IocpUdpSocket::Send(const Packet &packet, unsigned int waitTimeInMilliSec,SendStatus *sendStatus)
{
	epl::LockObj lock(m_baseSocketLock);
	return BaseUdpSocket::Send(packet,waitTimeInMilliSec,sendStatus);
}

Packet *IocpUdpSocket::Receive(unsigned int waitTimeInMilliSec,ReceiveStatus *retStatus)
{
	epl::LockObj lock(m_baseSocketLock);
	if(!IsConnectionAlive())
	{
		if(retStatus)
			*retStatus=RECEIVE_STATUS_FAIL_NOT_CONNECTED;
		return NULL;
	}

	// receive routine
	unsigned int packetSize=0;

	m_listLock->Lock();
	if(m_packetList.size()==0)
	{
		m_listLock->Unlock();
		if(!m_packetReceivedEvent.WaitForEvent(waitTimeInMilliSec))
		{
			if(retStatus)
				*retStatus=RECEIVE_STATUS_FAIL_TIME_OUT;
			return NULL;
		}

	}
	Packet *packet= m_packetList.front();
	packetSize=packet->GetPacketByteSize();
	if(packetSize==0)
	{
		m_listLock->Unlock();
		killConnection();
		if(retStatus)
			*retStatus=RECEIVE_STATUS_FAIL_CONNECTION_CLOSING;
		return NULL;
	}
	m_packetList.pop();
	m_listLock->Unlock();
	if(retStatus)
		*retStatus=RECEIVE_STATUS_SUCCESS;
	return packet;

}

void IocpUdpSocket::addPacket(Packet *packet)
{
	if(packet)
		packet->RetainObj();
	epl::LockObj lock(m_listLock);
	m_packetList.push(packet);
	m_packetReceivedEvent.SetEvent();
}

void IocpUdpSocket::execute()
{
	m_callBackObj->OnNewConnection(this);
}


