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
	m_waitTime=waitTimeMilliSec;
	m_threadStopEvent=EventEx(false,false);
	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_listLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_listLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_listLock=EP_NEW epl::NoLock();
		break;
	default:
		m_listLock=NULL;
		break;
	}
	Start();
}
ServerObjectRemover::ServerObjectRemover(const ServerObjectRemover& b):Thread(b),SmartObject(b)
{
	m_waitTime=b.m_waitTime;
	m_threadStopEvent=b.m_threadStopEvent;
	m_objectList=b.m_objectList;
	m_lockPolicy=b.m_lockPolicy;
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_listLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_listLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_listLock=EP_NEW epl::NoLock();
		break;
	default:
		m_listLock=NULL;
		break;
	}
	m_threadStopEvent.ResetEvent();
	Start();
}
ServerObjectRemover::~ServerObjectRemover()
{
	stopRemover();
	/// Not Releasing the object in the queue will cause memory leak
	/// Not recommend to use waitTimeMilliSec other than WAITTIME_INFINITE
	if(m_listLock)
		EP_DELETE m_listLock;
}

ServerObjectRemover & ServerObjectRemover::operator=(const ServerObjectRemover&b)
{
	if(this!=&b)
	{		
		stopRemover();
		if(m_listLock)
			EP_DELETE m_listLock;
		m_listLock=NULL;
		SmartObject::operator =(b);
		Thread::operator=(b);
		
		m_waitTime=b.m_waitTime;
		m_threadStopEvent=b.m_threadStopEvent;
		m_objectList=b.m_objectList;
		m_lockPolicy=b.m_lockPolicy;
		switch(m_lockPolicy)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_listLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_listLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_listLock=EP_NEW epl::NoLock();
			break;
		default:
			m_listLock=NULL;
			break;
		}
		m_threadStopEvent.ResetEvent();
		Start();
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
	if(GetStatus()==Thread::THREAD_STATUS_SUSPENDED)
		Resume();
}
void ServerObjectRemover::execute()
{
	while(1)
	{
		m_listLock->Lock();

		if(m_threadStopEvent.WaitForEvent(WAITTIME_IGNORE))
		{
			m_listLock->Unlock();
			break;
		}

		if(!m_objectList.size())
		{
			m_listLock->Unlock();
			Suspend();
			continue;
		}
		while(m_objectList.size())
		{
			BaseServerObject* serverObj=m_objectList.front();
			m_objectList.pop();
			m_listLock->Unlock();
			
			serverObj->ReleaseObj();
			m_listLock->Lock();
		}
		m_listLock->Unlock();
	}
}

void ServerObjectRemover::stopRemover()
{
	if(GetStatus()==Thread::THREAD_STATUS_TERMINATED)
		return;
	m_threadStopEvent.SetEvent();
	if(GetStatus()==Thread::THREAD_STATUS_SUSPENDED)
		Resume();
	TerminateAfter(m_waitTime);
}
