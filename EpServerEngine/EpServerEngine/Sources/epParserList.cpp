/*! 
ParserList for the EpServerEngine
Copyright (C) 2013  Woong Gyu La <juhgiyo@gmail.com>

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
#include "epParserList.h"

using namespace epse;

ParserList::ParserList(SyncPolicy syncPolicy,unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType):ServerObjectList(lockPolicyType),Thread(lockPolicyType),SmartObject(lockPolicyType)
{
	m_shouldTerminate=false;
	m_waitTime=waitTimeMilliSec;
	m_syncPolicy=syncPolicy;
	m_event=EventEx(false,false);
}
ParserList::ParserList(const ParserList& b):ServerObjectList(b),Thread(b),SmartObject(b)
{
	m_shouldTerminate=false;
	m_waitTime=b.m_waitTime;
	m_syncPolicy=b.m_syncPolicy;
	m_event=EventEx(false,false);
}
ParserList::~ParserList()
{
	StopParse();
}
ParserList & ParserList::operator=(const ParserList&b)
{
	if(this!=&b)
	{
		ServerObjectList::operator =(b);
		SmartObject::operator =(b);
		epl::LockObj lock(m_listLock);
		Thread::operator=(b);
		m_waitTime=b.m_waitTime;
		m_syncPolicy=b.m_syncPolicy;
	}
	return *this;
}

void ParserList::SetWaitTime(unsigned int milliSec)
{
	epl::LockObj lock(m_listLock);
	m_waitTime=milliSec;
}
unsigned int ParserList::GetWaitTime()
{
	epl::LockObj lock(m_listLock);
	return m_waitTime;
}
void ParserList::execute()
{
	while(1)
	{
		m_listLock->Lock();

		if(m_shouldTerminate)
		{
			m_listLock->Unlock();
			break;
		}

		if(!m_objectList.size())
		{
			m_listLock->Unlock();
			m_event.WaitForEvent();
			continue;
		}
		BasePacketParser* parser=(BasePacketParser*)m_objectList.front();
		m_objectList.erase(m_objectList.begin());
		m_listLock->Unlock();

		parser->ParsePacket(*(parser->GetPacketReceived()));
		m_serverObjRemover.Push(parser);
	}
}

void ParserList::RemoveTerminated()
{
	if(m_syncPolicy==SYNC_POLICY_ASYNCHRONOUS)
		ServerObjectList::RemoveTerminated();
	return;
}
bool ParserList::StartParse()
{
	if(GetStatus()!=Thread::THREAD_STATUS_TERMINATED)
		return false;
	Clear();
	m_listLock->Lock();
	m_shouldTerminate=false;
	m_listLock->Unlock();
	return Start();
}
void ParserList::StopParse()
{
	if(GetStatus()==Thread::THREAD_STATUS_TERMINATED)
		return;
	m_listLock->Lock();
	m_shouldTerminate=true;
	m_listLock->Unlock();
	m_event.SetEvent();
	TerminateAfter(m_waitTime);
}

void ParserList::Push(BaseServerObject* obj)
{
	ServerObjectList::Push(obj);
	m_event.SetEvent();
}
void ParserList::setSyncPolicy(SyncPolicy syncPolicy)
{
	m_syncPolicy=syncPolicy;
}