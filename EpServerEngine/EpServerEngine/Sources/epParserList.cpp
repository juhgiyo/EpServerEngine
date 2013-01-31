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
}
ParserList::ParserList(const ParserList& b):ServerObjectList(b),Thread(b),SmartObject(b)
{
	m_shouldTerminate=false;
	m_waitTime=b.m_waitTime;
	m_syncPolicy=b.m_syncPolicy;
}
ParserList::~ParserList()
{
	StopParser();
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
			Sleep(0);
			continue;
		}
		BasePacketParser* parser=(BasePacketParser*)m_objectList.front();
		m_objectList.erase(m_objectList.begin());
		parser->ParsePacket(*(parser->GetPacketReceived()));
		parser->ReleaseObj();
		m_listLock->Unlock();

	}
}

void ParserList::RemoveTerminated()
{
	if(m_syncPolicy==SYNC_POLICY_ASYNCHRONOUS)
		ServerObjectList::RemoveTerminated();
	return;
}
void ParserList::StartParse()
{
	Clear();
	Start();
}
void ParserList::StopParser()
{
	m_listLock->Lock();
	m_shouldTerminate=true;
	m_listLock->Unlock();
	TerminateAfter(m_waitTime);
}
void ParserList::setSyncPolicy(SyncPolicy syncPolicy)
{
	m_syncPolicy=syncPolicy;
}