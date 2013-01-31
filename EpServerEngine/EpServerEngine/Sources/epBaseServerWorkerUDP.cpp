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
BaseServerWorkerUDP::BaseServerWorkerUDP(unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType): BaseServerSendObject(waitTimeMilliSec,lockPolicyType)
{
	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_lock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_lock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_lock=EP_NEW epl::NoLock();
		break;
	default:
		m_lock=NULL;
		break;
	}
	m_packet=NULL;
	m_server=NULL;
	m_maxPacketSize=0;
	m_parser=NULL;
	m_parserList=NULL;
}
BaseServerWorkerUDP::BaseServerWorkerUDP(const BaseServerWorkerUDP& b) : BaseServerSendObject(b)
{
	m_packet=NULL;
	m_server=NULL;
	m_parser=NULL;
	m_maxPacketSize=0;
	m_lockPolicy=b.m_lockPolicy;
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_lock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_lock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_lock=EP_NEW epl::NoLock();
		break;
	default:
		m_lock=NULL;
		break;
	}
	m_parserList=NULL;
}

BaseServerWorkerUDP::~BaseServerWorkerUDP()
{
	if(m_parser)
		m_parser->ReleaseObj();
	if(m_parserList)
	{
		m_parserList->ReleaseObj();
	}
	WaitFor(m_waitTime);
	m_lock->Lock();
	if(m_packet)
	{
		m_packet->ReleaseObj();	
		m_packet=NULL;
	}
	m_lock->Unlock();

	if(m_lock)
		EP_DELETE m_lock;
}

void BaseServerWorkerUDP::setArg(void* a)
{
	epl::LockObj lock(m_lock);
	PacketPassUnit *clientSocket=reinterpret_cast<PacketPassUnit*>(a);
	m_clientSocket=clientSocket->m_clientSocket;
	m_server=clientSocket->m_server;
	
	if(m_packet)
		m_packet->ReleaseObj();
	m_packet=clientSocket->m_packet;
	if(m_packet)
		m_packet->RetainObj();

	m_maxPacketSize=m_server->GetMaxPacketByteSize();
}

int BaseServerWorkerUDP::Send(const Packet &packet)
{
	epl::LockObj lock(m_lock);
	EP_ASSERT(packet.GetPacketByteSize()<=m_maxPacketSize);
	if(m_server)
		return m_server->send(packet,m_clientSocket);
	return 0;
}

void BaseServerWorkerUDP::setServer(BaseServerUDP *server)
{
	epl::LockObj lock(m_lock);
	m_server=server;
}

void BaseServerWorkerUDP::setParserList(ParserList *parserList)
{
	EP_ASSERT(parserList);
	if(m_parserList)
		m_parserList->ReleaseObj();
	m_parserList=parserList;
	parserList->RetainObj();

}

void BaseServerWorkerUDP::execute()
{
	BasePacketParser::PacketPassUnit passUnit;
	passUnit.m_packet=m_packet;
	passUnit.m_this=this;
	m_parser =createNewPacketParser();
	m_parser->setSyncPolicy(m_syncPolicy);
	if(m_syncPolicy==SYNC_POLICY_ASYNCHRONOUS||m_syncPolicy==SYNC_POLICY_SYNCHRONOUS_BY_CLIENT)
	{
		m_parser->Start(reinterpret_cast<void*>(&passUnit));
		m_parser->WaitFor(m_waitTime);
	}
	else
	{
		m_parser->setPacketPassUnit(&passUnit);
		m_parserList->Push(m_parser);
	}

}

