/*! 
BasePacketParser for the EpServerEngine
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
#include "epBasePacketParser.h"

using namespace epse;

BasePacketParser::BasePacketParser(unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType):BaseServerObject(waitTimeMilliSec,lockPolicyType)
{
	m_threadStopEvent=EventEx(false,false);
	m_owner=NULL;
	m_packetReceived=NULL;
	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_generalLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_generalLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_generalLock=EP_NEW epl::NoLock();
		break;
	default:
		m_generalLock=NULL;
		break;
	}
}

BasePacketParser::BasePacketParser(const BasePacketParser& b):BaseServerObject(b)
{
	m_threadStopEvent=b.m_threadStopEvent;
	m_owner=b.m_owner;
	if(m_owner)
		m_owner->RetainObj();
	m_packetReceived=b.m_packetReceived;
	if(m_packetReceived)
		m_packetReceived->RetainObj();
	m_lockPolicy=b.m_lockPolicy;
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_generalLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_generalLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_generalLock=EP_NEW epl::NoLock();
		break;
	default:
		m_generalLock=NULL;
		break;
	}
}
BasePacketParser::~BasePacketParser()
{
	resetParser();
}

BasePacketParser & BasePacketParser::operator=(const BasePacketParser&b)
{
	if(this!=&b)
	{
		resetParser();

		BaseServerObject::operator =(b);

		m_threadStopEvent=b.m_threadStopEvent;
		m_owner=b.m_owner;
		if(m_owner)
			m_owner->RetainObj();
		m_packetReceived=b.m_packetReceived;
		if(m_owner)
			m_packetReceived->RetainObj();
		m_lockPolicy=b.m_lockPolicy;
		switch(m_lockPolicy)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_generalLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_generalLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_generalLock=EP_NEW epl::NoLock();
			break;
		default:
			m_generalLock=NULL;
			break;
		}

	}
	return *this;
}

void BasePacketParser::resetParser()
{
	if(GetStatus()!=Thread::THREAD_STATUS_TERMINATED)
	{
		m_threadStopEvent.SetEvent();
		if(GetStatus()==Thread::THREAD_STATUS_SUSPENDED)
			Resume();
		TerminateAfter(m_waitTime);
	}

	m_generalLock->Lock();
	if(m_owner)
		m_owner->ReleaseObj();
	if(m_packetReceived)
		m_packetReceived->ReleaseObj();
	m_generalLock->Unlock();

	if(m_generalLock)
		EP_DELETE m_generalLock;
	
	m_owner=NULL;
	m_packetReceived=NULL;
	m_generalLock=NULL;
}

int BasePacketParser::Send(const Packet &packet)
{
	epl::LockObj lock(m_generalLock);
	if(m_owner)
	{
		return m_owner->Send(packet);
	}
	return 0;
}

void BasePacketParser::execute()
{
	if(m_packetReceived)
	{
		m_threadStopEvent.ResetEvent();
		ParsePacket(*m_packetReceived);
	}
}

const Packet* BasePacketParser::GetPacketReceived()
{
	return m_packetReceived;
}
void BasePacketParser::setPacketPassUnit(const PacketPassUnit& packetPassUnit)
{
	epl::LockObj lock(m_generalLock);
	if(m_packetReceived)
		m_packetReceived->ReleaseObj();
	m_packetReceived=packetPassUnit.m_packet;
	if(m_packetReceived)
		m_packetReceived->RetainObj();

	if(m_owner)
		m_owner->ReleaseObj();
	m_owner=packetPassUnit.m_owner;
	if(m_owner)
		m_owner->RetainObj();
}