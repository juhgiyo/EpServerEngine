/*! 
ServerObjectRemover for the EpServerEngine
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
#include "epServerObjectRemover.h"

using namespace epse;

ServerObjectRemover::ServerObjectRemover(unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType):Thread(lockPolicyType),SmartObject(lockPolicyType)
{
	m_shouldTerminate=false;
	m_waitTime=waitTimeMilliSec;
	m_event=EventEx(false,false);
}
ServerObjectRemover::ServerObjectRemover(const ServerObjectRemover& b):Thread(b),SmartObject(b)
{
	m_shouldTerminate=false;
	m_waitTime=b.m_waitTime;
	m_event=EventEx(false,false);
	m_objectList=b.m_objectList;
	ServerObjectRemover & unSafeB=const_cast<ServerObjectRemover&>(b);
	while(unSafeB.m_objectList.size())
		unSafeB.m_objectList.pop();
}
ServerObjectRemover::~ServerObjectRemover()
{
	stopRemover();
	Clear();
}

ServerObjectRemover & ServerObjectRemover::operator=(const ServerObjectRemover&b)
{
	if(this!=&b)
	{
		SmartObject::operator =(b);
		epl::LockObj lock(m_listLock);
		Thread::operator=(b);
		m_waitTime=b.m_waitTime;
		m_objectList=b.m_objectList;
		ServerObjectRemover & unSafeB=const_cast<ServerObjectRemover&>(b);
		while(unSafeB.m_objectList.size())
			unSafeB.m_objectList.pop();
	}
	return *this;
}

void ServerObjectRemover::SetWaitTime(unsigned int milliSec)
{
	epl::LockObj lock(m_listLock);
	m_waitTime=milliSec;
}
unsigned int ServerObjectRemover::GetWaitTime()
{
	epl::LockObj lock(m_listLock);
	return m_waitTime;
}

void ServerObjectRemover::Push(BaseServerObject* obj)
{
	m_listLock->Lock();
	m_objectList.push(obj);
	m_listLock->Unlock();
	m_event.SetEvent();
}
void ServerObjectRemover::execute()
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
		BaseServerObject* serverObj=m_objectList.front();
		m_objectList.pop();
		m_listLock->Unlock();

		serverObj->ReleaseObj();
	}
}

void ServerObjectRemover::stopRemover()
{
	if(GetStatus()==Thread::THREAD_STATUS_TERMINATED)
		return;
	m_listLock->Lock();
	m_shouldTerminate=true;
	m_listLock->Unlock();
	m_event.SetEvent();
	TerminateAfter(m_waitTime);
}

void ServerObjectRemover::Clear()
{
	epl::LockObj lock(m_listLock);
	while(!m_objectList.size())
	{
		BaseServerObject* serverObj=m_objectList.front();
		m_objectList.pop();
		serverObj->ReleaseObj();
	}
}