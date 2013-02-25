/*! 
BaseServerWorkerUDP for the EpServerEngine
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
#include "epBaseServerWorkerUDP.h"
#include "epBaseServerUDP.h"
using namespace epse;
BaseServerWorkerUDP::BaseServerWorkerUDP(unsigned int maximumParserCount,unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType): BaseServerSendObject(waitTimeMilliSec,lockPolicyType)
{
	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_baseWorkerLock=EP_NEW epl::CriticalSectionEx();
		m_killConnectionLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_baseWorkerLock=EP_NEW epl::Mutex();
		m_killConnectionLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_baseWorkerLock=EP_NEW epl::NoLock();
		m_killConnectionLock=EP_NEW epl::NoLock();
		break;
	default:
		m_baseWorkerLock=NULL;
		m_killConnectionLock=NULL;
		break;
	}
	m_packet=NULL;
	m_server=NULL;
	m_maxPacketSize=0;
	m_maxParserCount=maximumParserCount;
	m_parser=NULL;
	m_parserList=NULL;
}
BaseServerWorkerUDP::BaseServerWorkerUDP(const BaseServerWorkerUDP& b) : BaseServerSendObject(b)
{
	m_lockPolicy=b.m_lockPolicy;
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_baseWorkerLock=EP_NEW epl::CriticalSectionEx();
		m_killConnectionLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_baseWorkerLock=EP_NEW epl::Mutex();
		m_killConnectionLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_baseWorkerLock=EP_NEW epl::NoLock();
		m_killConnectionLock=EP_NEW epl::NoLock();
		break;
	default:
		m_baseWorkerLock=NULL;
		m_killConnectionLock=NULL;
		break;
	}

	m_maxPacketSize=b.m_maxPacketSize;
	m_maxParserCount=b.m_maxParserCount;
	m_server=b.m_server;
	m_packet=b.m_packet;
	if(m_packet)
		m_packet->RetainObj();
	m_parser=b.m_parser;
	if(m_parser)
		m_parser->RetainObj();
	m_parserList=b.m_parserList;
	if(m_parserList)
		m_parserList->RetainObj();
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
			m_killConnectionLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_baseWorkerLock=EP_NEW epl::Mutex();
			m_killConnectionLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_baseWorkerLock=EP_NEW epl::NoLock();
			m_killConnectionLock=EP_NEW epl::NoLock();
			break;
		default:
			m_baseWorkerLock=NULL;
			m_killConnectionLock=NULL;
			break;
		}

		m_maxPacketSize=b.m_maxPacketSize;
		m_maxParserCount=b.m_maxParserCount;
		m_server=b.m_server;
		m_packet=b.m_packet;
		if(m_packet)
			m_packet->RetainObj();
		m_parser=b.m_parser;
		if(m_parser)
			m_parser->RetainObj();
		m_parserList=b.m_parserList;
		if(m_parserList)
			m_parserList->RetainObj();
	}
	return *this;
}

void BaseServerWorkerUDP::resetWorker()
{
	KillConnection();

	if(m_parser)
		m_parser->ReleaseObj();
	if(m_parserList)
		m_parserList->ReleaseObj();
	if(m_packet)
		m_packet->ReleaseObj();	
	if(m_baseWorkerLock)
		EP_DELETE m_baseWorkerLock;

	if(m_killConnectionLock)
		EP_DELETE m_killConnectionLock;

	m_parser=NULL;
	m_parserList=NULL;
	m_packet=NULL;
	m_baseWorkerLock=NULL;
	m_killConnectionLock=NULL;
}
void BaseServerWorkerUDP::GetMaximumParserCount(unsigned int maxParserCount)
{
	epl::LockObj lock(m_baseWorkerLock);
	m_maxParserCount=maxParserCount;

}
unsigned int BaseServerWorkerUDP::GetMaximumParserCount() const
{
	epl::LockObj lock(m_baseWorkerLock);
	return m_maxParserCount;
}

void BaseServerWorkerUDP::setPacketPassUnit(const PacketPassUnit &packetPassUnit)
{
	epl::LockObj lock(m_baseWorkerLock);
	m_clientSocket=packetPassUnit.m_clientSocket;
	m_server=packetPassUnit.m_server;

	if(m_packet)
		m_packet->ReleaseObj();
	m_packet=packetPassUnit.m_packet;
	if(m_packet)
		m_packet->RetainObj();

	m_maxPacketSize=m_server->GetMaxPacketByteSize();
}

int BaseServerWorkerUDP::Send(const Packet &packet)
{
	epl::LockObj lock(m_baseWorkerLock);
	EP_ASSERT(packet.GetPacketByteSize()<=m_maxPacketSize);
	if(m_server)
		return m_server->send(packet,m_clientSocket);
	return 0;
}

bool BaseServerWorkerUDP::IsConnectionAlive() const
{
	return (GetStatus()==Thread::THREAD_STATUS_STARTED);
}

void BaseServerWorkerUDP::KillConnection()
{
	epl::LockObj lock(m_baseWorkerLock);
	if(!IsConnectionAlive())
	{
		return;
	}
	killConnection(false);
}


void BaseServerWorkerUDP::killConnection(bool fromInternal)
{
	if(!m_killConnectionLock->TryLock())
	{
		return;
	}
	if(IsConnectionAlive())
	{
		if(!fromInternal)
			TerminateAfter(m_waitTime);
		RemoveSelfFromContainer();
		if(m_parser)
		{
			m_parser->ReleaseObj();
			m_parser=NULL;
		}
	}

	m_killConnectionLock->Unlock();
}


void BaseServerWorkerUDP::setServer(BaseServerUDP *server)
{
	epl::LockObj lock(m_baseWorkerLock);
	m_server=server;
}

void BaseServerWorkerUDP::setParserList(ParserList *parserList)
{
	EP_ASSERT(parserList);
	if(m_parserList)
		m_parserList->ReleaseObj();
	m_parserList=parserList;
	if(m_parserList)
		m_parserList->RetainObj();

}

void BaseServerWorkerUDP::execute()
{
	BasePacketParser::PacketPassUnit passUnit;
	passUnit.m_packet=m_packet;
	passUnit.m_owner=this;
	if(m_parser)
	{
		m_parser->ReleaseObj();
		m_parser=NULL;
	}
	m_parser =createNewPacketParser();
	m_parser->setSyncPolicy(m_syncPolicy);
	m_parser->setPacketPassUnit(passUnit);
	if(m_syncPolicy==SYNC_POLICY_ASYNCHRONOUS||m_syncPolicy==SYNC_POLICY_SYNCHRONOUS_BY_CLIENT)
	{
		m_parser->Start();
		m_parser->WaitFor(WAITTIME_INIFINITE);
		m_parser->ReleaseObj();
		m_parser=NULL;
	}
	else
	{
		if(m_parserList)
		{
			if(GetMaximumParserCount()!=PARSER_LIMIT_INFINITE)
			{
				while(m_parserList->Count()>=GetMaximumParserCount())
				{
					m_parserList->WaitForListSizeDecrease();
				}
			}
			m_parserList->Push(m_parser);
			m_parserList->ReleaseObj();
			m_parserList=NULL;
		}
		
	}
	killConnection(true);

}

