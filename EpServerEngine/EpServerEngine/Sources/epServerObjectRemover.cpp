/*! 
ServerObjectRemover for the EpServerEngine

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
#include "epServerObjectRemover.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

ServerObjectRemover::ServerObjectRemover(unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType):Thread(EP_THREAD_PRIORITY_NORMAL,lockPolicyType),SmartObject(lockPolicyType)
{
	m_waitTime=waitTimeMilliSec;
	m_threadStopEvent=EventEx(false,false);
	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_listLock=EP_NEW epl::CriticalSectionEx();
		m_stopLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_listLock=EP_NEW epl::Mutex();
		m_stopLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_listLock=EP_NEW epl::NoLock();
		m_stopLock=EP_NEW epl::NoLock();
		break;
	default:
		m_listLock=NULL;
		m_stopLock=NULL;
		break;
	}
	Start(TRHEAD_OPCODE_CREATE_SUSPEND);
}
ServerObjectRemover::ServerObjectRemover(const ServerObjectRemover& b):Thread(b),SmartObject(b)
{
	m_lockPolicy=b.m_lockPolicy;
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_listLock=EP_NEW epl::CriticalSectionEx();
		m_stopLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_listLock=EP_NEW epl::Mutex();
		m_stopLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_listLock=EP_NEW epl::NoLock();
		m_stopLock=EP_NEW epl::NoLock();
		break;
	default:
		m_listLock=NULL;
		m_stopLock=NULL;
		break;
	}

	m_waitTime=b.m_waitTime;
	m_threadStopEvent=b.m_threadStopEvent;
	
	ServerObjectRemover&unSafeB=const_cast<ServerObjectRemover&>(b);
	unSafeB.m_listLock->Lock();
	m_objectList=b.m_objectList;
	while(unSafeB.m_objectList.size())
	{
		unSafeB.m_objectList.pop();
	}
	unSafeB.m_listLock->Unlock();

	m_threadStopEvent.ResetEvent();
	Start(TRHEAD_OPCODE_CREATE_SUSPEND);
}
ServerObjectRemover::~ServerObjectRemover()
{
	stopRemover();
	/// Not Releasing the object in the queue will cause memory leak
	/// Not recommend to use waitTimeMilliSec other than WAITTIME_INFINITE
	while(m_objectList.size())
	{
		BaseServerObject* serverObj=m_objectList.front();
		m_objectList.pop();
		serverObj->ReleaseObj();
	}
	if(m_listLock)
		EP_DELETE m_listLock;
	m_listLock=NULL;
	if(m_stopLock)
		EP_DELETE m_stopLock;
	m_stopLock=NULL;
}

ServerObjectRemover & ServerObjectRemover::operator=(const ServerObjectRemover&b)
{
	if(this!=&b)
	{		
		stopRemover();
		while(m_objectList.size())
		{
			BaseServerObject* serverObj=m_objectList.front();
			m_objectList.pop();
			serverObj->ReleaseObj();
		}
		if(m_listLock)
			EP_DELETE m_listLock;
		m_listLock=NULL;
		if(m_stopLock)
			EP_DELETE m_stopLock;
		m_stopLock=NULL;

		SmartObject::operator =(b);
		Thread::operator=(b);
	
		m_lockPolicy=b.m_lockPolicy;
		switch(m_lockPolicy)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_listLock=EP_NEW epl::CriticalSectionEx();
			m_stopLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_listLock=EP_NEW epl::Mutex();
			m_stopLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_listLock=EP_NEW epl::NoLock();
			m_stopLock=EP_NEW epl::NoLock();
			break;
		default:
			m_listLock=NULL;
			m_stopLock=NULL;
			break;
		}

		m_waitTime=b.m_waitTime;
		m_threadStopEvent=b.m_threadStopEvent;

		ServerObjectRemover&unSafeB=const_cast<ServerObjectRemover&>(b);
		unSafeB.m_listLock->Lock();
		m_objectList=b.m_objectList;
		while(unSafeB.m_objectList.size())
		{
			unSafeB.m_objectList.pop();
		}
		unSafeB.m_listLock->Unlock();
	
		m_threadStopEvent.ResetEvent();
		Start(TRHEAD_OPCODE_CREATE_SUSPEND);
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
			m_listLock->Lock();
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
	if(!m_stopLock->TryLock())
	{
		WaitFor(m_waitTime);
		return;
	}
	if(GetStatus()==Thread::THREAD_STATUS_TERMINATED)
	{
		m_stopLock->Unlock();
		return;
	}
	m_threadStopEvent.SetEvent();
	if(GetStatus()==Thread::THREAD_STATUS_SUSPENDED)
		Resume();
	TerminateAfter(m_waitTime);
	m_stopLock->Unlock();
}
