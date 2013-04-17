/*! 
AsyncUdpSocket for the EpServerEngine
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
#include "epAsyncUdpSocket.h"
#include "epAsyncUdpServer.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;
AsyncUdpSocket::AsyncUdpSocket(ServerCallbackInterface *callBackObj,bool isAsynchronousReceive,unsigned int waitTimeMilliSec,unsigned int maximumProcessorCount,epl::LockPolicy lockPolicyType): BaseUdpSocket(callBackObj,waitTimeMilliSec,lockPolicyType)
{
	m_processorList=ServerObjectList(waitTimeMilliSec,lockPolicyType);
	m_threadStopEvent=EventEx(false,false);
	m_maxProcessorCount=maximumProcessorCount;
	m_isAsynchronousReceive=isAsynchronousReceive;
}

AsyncUdpSocket::~AsyncUdpSocket()
{
	KillConnection();
}
void AsyncUdpSocket::SetMaximumProcessorCount(unsigned int maxProcessorCount)
{
	epl::LockObj lock(m_baseSocketLock);
	m_maxProcessorCount=maxProcessorCount;

}
unsigned int AsyncUdpSocket::GetMaximumProcessorCount() const
{
	epl::LockObj lock(m_baseSocketLock);
	return m_maxProcessorCount;
}

bool AsyncUdpSocket::GetIsAsynchronousReceive() const
{
	return m_isAsynchronousReceive;
}
void AsyncUdpSocket::SetIsAsynchronousReceive(bool isASynchronousReceive)
{
	m_isAsynchronousReceive=isASynchronousReceive;
}

void AsyncUdpSocket::SetWaitTime(unsigned int milliSec)
{
	m_waitTime=milliSec;
	m_processorList.SetWaitTime(milliSec);
}

void AsyncUdpSocket::KillConnection()
{
	epl::LockObj lock(m_baseSocketLock);
	if(!IsConnectionAlive())
	{
		return;
	}
	m_threadStopEvent.SetEvent();
	if(GetStatus()==Thread::THREAD_STATUS_SUSPENDED)
		Resume();
	TerminateAfter(m_waitTime);
	m_processorList.Clear();

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

void AsyncUdpSocket::killConnection()
{
	if(IsConnectionAlive())
	{

		m_processorList.Clear();
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

void AsyncUdpSocket::addPacket(Packet *packet)
{
	if(packet)
		packet->RetainObj();
	epl::LockObj lock(m_listLock);
	m_packetList.push(packet);
	if(GetStatus()==THREAD_STATUS_SUSPENDED)
		Resume();
}

void AsyncUdpSocket::execute()
{
	m_callBackObj->OnNewConnection(this);

	unsigned int packetSize=0;
	while(1)
	{
		if(m_threadStopEvent.WaitForEvent(WAITTIME_IGNORE))
		{
			break;
		}
		m_listLock->Lock();
		if(m_packetList.size()==0)
		{
			m_listLock->Unlock();
			Suspend();
			continue;
		}
		Packet *packet= m_packetList.front();
		packetSize=packet->GetPacketByteSize();
		if(packetSize==0)
		{
			m_listLock->Unlock();
			break;
		}

		m_packetList.pop();
		m_listLock->Unlock();

		if(m_isAsynchronousReceive)
		{
			ServerPacketProcessor::PacketPassUnit passUnit;
			passUnit.m_owner=this;
			passUnit.m_packet=packet;
			ServerPacketProcessor *parser =EP_NEW ServerPacketProcessor(m_callBackObj,m_waitTime,m_lockPolicy);
			parser->setPacketPassUnit(passUnit);
			m_processorList.Push(parser);
			parser->Start();
			packet->ReleaseObj();
			parser->ReleaseObj();
			if(GetMaximumProcessorCount()!=PROCESSOR_LIMIT_INFINITE)
			{
				while(m_processorList.Count()>=GetMaximumProcessorCount())
				{
					m_processorList.WaitForListSizeDecrease();
				}
			}
		}
		else
		{
			m_callBackObj->OnReceived(this,*packet);
			packet->ReleaseObj();
		}

	}	

	killConnection();

}

