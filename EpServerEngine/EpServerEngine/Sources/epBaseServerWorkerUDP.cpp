/*! 
BaseServerWorkerUDP for the EpServerEngine

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
#include "epBaseServerWorkerUDP.h"
#include "epBaseServerUDP.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)


using namespace epse;
BaseServerWorkerUDP::BaseServerWorkerUDP(unsigned int maximumParserCount,unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType): BaseServerSendObject(waitTimeMilliSec,lockPolicyType)
{
	m_threadStopEvent=EventEx(false,false);
	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_baseWorkerLock=EP_NEW epl::CriticalSectionEx();
		m_listLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_baseWorkerLock=EP_NEW epl::Mutex();
		m_listLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_baseWorkerLock=EP_NEW epl::NoLock();
		m_listLock=EP_NEW epl::NoLock();
		break;
	default:
		m_baseWorkerLock=NULL;
		m_listLock=NULL;
		break;
	}
	m_server=NULL;
	m_maxPacketSize=0;
	m_maxParserCount=maximumParserCount;
	m_parserList=NULL;
}
BaseServerWorkerUDP::BaseServerWorkerUDP(const BaseServerWorkerUDP& b) : BaseServerSendObject(b)
{

	m_lockPolicy=b.m_lockPolicy;
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_baseWorkerLock=EP_NEW epl::CriticalSectionEx();
		m_listLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_baseWorkerLock=EP_NEW epl::Mutex();
		m_listLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_baseWorkerLock=EP_NEW epl::NoLock();
		m_listLock=EP_NEW epl::NoLock();
		break;
	default:
		m_baseWorkerLock=NULL;
		m_listLock=NULL;
		break;
	}

	m_maxPacketSize=b.m_maxPacketSize;
	m_maxParserCount=b.m_maxParserCount;
	m_server=b.m_server;
	vector<Packet*>::iterator iter;
	BaseServerWorkerUDP &unSafeB =const_cast<BaseServerWorkerUDP&>(b);
	unSafeB.m_listLock->Lock();
	for(iter=unSafeB.m_packetList.begin();iter!=unSafeB.m_packetList.end();iter++)
	{
		if(*iter)
		{
			(*iter)->RetainObj();
			m_packetList.push_back(*iter);
		}
	}
	unSafeB.m_listLock->Unlock();
	m_parserList=b.m_parserList;
	if(m_parserList)
		m_parserList->RetainObj();
	m_threadStopEvent=b.m_threadStopEvent;
	m_threadStopEvent.ResetEvent();
}

BaseServerWorkerUDP::~BaseServerWorkerUDP()
{
	resetWorker();
}

BaseServerWorkerUDP & BaseServerWorkerUDP::operator=(const BaseServerWorkerUDP&b)
{
	if(this!=&b)
	{
		resetWorker();
		
		BaseServerSendObject::operator =(b);
		
		m_lockPolicy=b.m_lockPolicy;
		switch(m_lockPolicy)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_baseWorkerLock=EP_NEW epl::CriticalSectionEx();
			m_listLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_baseWorkerLock=EP_NEW epl::Mutex();
			m_listLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_baseWorkerLock=EP_NEW epl::NoLock();
			m_listLock=EP_NEW epl::NoLock();
			break;
		default:
			m_baseWorkerLock=NULL;
			m_listLock=NULL;
			break;
		}

		m_maxPacketSize=b.m_maxPacketSize;
		m_maxParserCount=b.m_maxParserCount;
		m_server=b.m_server;
		vector<Packet*>::iterator iter;
		BaseServerWorkerUDP &unSafeB =const_cast<BaseServerWorkerUDP&>(b);
		unSafeB.m_listLock->Lock();
		for(iter=unSafeB.m_packetList.begin();iter!=unSafeB.m_packetList.end();iter++)
		{
			if(*iter)
			{
				(*iter)->RetainObj();
				m_packetList.push_back(*iter);
			}
		}
		unSafeB.m_listLock->Unlock();
		m_parserList=b.m_parserList;
		if(m_parserList)
			m_parserList->RetainObj();

		m_threadStopEvent=b.m_threadStopEvent;
		m_threadStopEvent.ResetEvent();
	}
	return *this;
}

void BaseServerWorkerUDP::resetWorker()
{
	KillConnection();

	if(m_parserList)
		m_parserList->ReleaseObj();
	m_parserList=NULL;

	m_listLock->Lock();
	vector<Packet*>::iterator iter;
	for(iter=m_packetList.begin();iter!=m_packetList.end();iter++)
	{
		if(*iter)
			(*iter)->ReleaseObj();
	}
	m_packetList.clear();
	m_listLock->Unlock();

	if(m_listLock)
		EP_DELETE m_listLock;
	m_listLock=NULL;

	if(m_baseWorkerLock)
		EP_DELETE m_baseWorkerLock;
	m_baseWorkerLock=NULL;


	
}
void BaseServerWorkerUDP::SetMaximumParserCount(unsigned int maxParserCount)
{
	epl::LockObj lock(m_baseWorkerLock);
	m_maxParserCount=maxParserCount;

}
unsigned int BaseServerWorkerUDP::GetMaximumParserCount() const
{
	epl::LockObj lock(m_baseWorkerLock);
	return m_maxParserCount;
}
void BaseServerWorkerUDP::addPacket(Packet *packet)
{
	if(packet)
		packet->RetainObj();
	epl::LockObj lock(m_listLock);
	m_packetList.push_back(packet);
	if(GetStatus()==THREAD_STATUS_SUSPENDED)
		Resume();
}
void BaseServerWorkerUDP::setPacketPassUnit(const PacketPassUnit &packetPassUnit)
{
	epl::LockObj lock(m_baseWorkerLock);
	m_clientSocket=packetPassUnit.m_clientSocket;
	m_server=packetPassUnit.m_server;
	m_maxPacketSize=m_server->GetMaxPacketByteSize();
}

int BaseServerWorkerUDP::Send(const Packet &packet, unsigned int waitTimeInMilliSec)
{
	epl::LockObj lock(m_baseWorkerLock);
	EP_ASSERT(packet.GetPacketByteSize()<=m_maxPacketSize);
	if(m_server)
		return m_server->send(packet,m_clientSocket,waitTimeInMilliSec);
	return 0;
}

vector<BaseServerObject*> BaseServerWorkerUDP::GetPacketParserList() const
{
	if(m_parserList)
		return m_parserList->GetList();
	return vector<BaseServerObject*>();
}

bool BaseServerWorkerUDP::IsConnectionAlive() const
{
	return (GetStatus()!=Thread::THREAD_STATUS_TERMINATED);
}

BaseServerUDP *BaseServerWorkerUDP::GetOwner() const
{
	return m_server;
}

epl::EpTString BaseServerWorkerUDP::GetIP() const
{
	sockaddr socketAddr=m_clientSocket;
	TCHAR ip[INET6_ADDRSTRLEN] = {0};
	unsigned long ipSize=INET6_ADDRSTRLEN;
	WSAAddressToString(&socketAddr,sizeof(sockaddr),NULL,ip,&ipSize);
	epl::EpTString retString=ip;
	return retString;
}
sockaddr BaseServerWorkerUDP::GetSockAddr() const
{
	return m_clientSocket;
}

unsigned int BaseServerWorkerUDP::GetMaxPacketByteSize() const
{
	return m_maxPacketSize;
}
void BaseServerWorkerUDP::KillConnection()
{
	epl::LockObj lock(m_baseWorkerLock);
	if(!IsConnectionAlive())
	{
		return;
	}
	m_threadStopEvent.SetEvent();
	if(GetStatus()==Thread::THREAD_STATUS_SUSPENDED)
		Resume();
	TerminateAfter(m_waitTime);

	if(m_parserList)
	{
		if(m_syncPolicy==SYNC_POLICY_SYNCHRONOUS_BY_CLIENT)
		{
			m_parserList->StopParse();
		}

		if(m_syncPolicy!=SYNC_POLICY_SYNCHRONOUS)
		{
			m_parserList->Clear();
		}

		m_parserList->ReleaseObj();
		m_parserList=NULL;
	}		

	m_listLock->Lock();
	vector<Packet*>::iterator iter;
	for(iter=m_packetList.begin();iter!=m_packetList.end();iter++)
	{
		if(*iter)
			(*iter)->ReleaseObj();
	}
	m_packetList.clear();
	m_listLock->Unlock();

	removeSelfFromContainer();

}


void BaseServerWorkerUDP::killConnection()
{
	if(IsConnectionAlive())
	{
	
		if(m_parserList)
		{
			if(m_syncPolicy==SYNC_POLICY_SYNCHRONOUS_BY_CLIENT)
			{
				m_parserList->StopParse();
			}

			if(m_syncPolicy!=SYNC_POLICY_SYNCHRONOUS)
			{
				m_parserList->Clear();
			}

			m_parserList->ReleaseObj();
			m_parserList=NULL;
		}		
		m_listLock->Lock();
		vector<Packet*>::iterator iter;
		for(iter=m_packetList.begin();iter!=m_packetList.end();iter++)
		{
			if(*iter)
				(*iter)->ReleaseObj();
		}
		m_packetList.clear();
		m_listLock->Unlock();

		removeSelfFromContainer();


	}
}


void BaseServerWorkerUDP::setServer(BaseServerUDP *server)
{
	epl::LockObj lock(m_baseWorkerLock);
	m_server=server;
}

void BaseServerWorkerUDP::setParserList(ParserList *parserList)
{
	if(!parserList)
		return;
	if(m_parserList)
		m_parserList->ReleaseObj();
	m_parserList=parserList;
	if(m_parserList)
		m_parserList->RetainObj();

}

void BaseServerWorkerUDP::execute()
{
	unsigned int packetSize=0;

	if(!m_parserList)
		m_parserList=EP_NEW ParserList(m_syncPolicy,m_waitTime,m_lockPolicy);

	if(m_parserList&&m_syncPolicy==SYNC_POLICY_SYNCHRONOUS_BY_CLIENT)
	{
		m_parserList->StartParse();
	}

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
		m_packetList.erase(m_packetList.begin());
		m_listLock->Unlock();

		BasePacketParser::PacketPassUnit passUnit;
		passUnit.m_owner=this;
		passUnit.m_packet=packet;
		BasePacketParser *parser =createNewPacketParser();
		if(!parser)
		{
			packet->ReleaseObj();
			continue;
		}
		parser->setSyncPolicy(m_syncPolicy);
		parser->setPacketPassUnit(passUnit);
		m_parserList->Push(parser);
		if(m_syncPolicy==SYNC_POLICY_ASYNCHRONOUS)
			parser->Start();
		packet->ReleaseObj();
		parser->ReleaseObj();
		if(GetMaximumParserCount()!=PARSER_LIMIT_INFINITE)
		{
			while(m_parserList->Count()>=GetMaximumParserCount())
			{
				m_parserList->WaitForListSizeDecrease();
			}
		}
	}	
	
	killConnection();

}

