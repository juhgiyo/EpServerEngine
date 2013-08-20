/*! 
AsyncUdpSocket for the EpServerEngine

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
	if(TerminateAfter(m_waitTime)==Thread::TERMINATE_RESULT_GRACEFULLY_TERMINATED)
		return;
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
			m_callBackObj->OnReceived(this,packet,RECEIVE_STATUS_SUCCESS);
			packet->ReleaseObj();
		}

	}	

	killConnection();

}

