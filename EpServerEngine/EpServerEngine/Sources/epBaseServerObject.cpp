/*! 
BaseServerObject for the EpServerEngine

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
#include "epBaseServerObject.h"
#include "epServerObjectList.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

BaseServerObject::BaseServerObject(unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType):epl::SmartObject(lockPolicyType),epl::Thread(EP_THREAD_PRIORITY_NORMAL,lockPolicyType)
{
	m_waitTime=waitTimeMilliSec;
	m_syncPolicy=SYNC_POLICY_ASYNCHRONOUS;
	m_lockPolicy=lockPolicyType;
	m_container=NULL;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_containerLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_containerLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_containerLock=EP_NEW epl::NoLock();
		break;
	default:
		m_containerLock=NULL;
		break;
	}
}

BaseServerObject::BaseServerObject(const BaseServerObject& b):SmartObject(b),Thread(b)
{
	m_waitTime=b.m_waitTime;
	m_syncPolicy=b.m_syncPolicy;
	m_container=b.m_container;
	m_lockPolicy=b.m_lockPolicy;
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_containerLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_containerLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_containerLock=EP_NEW epl::NoLock();
		break;
	default:
		m_containerLock=NULL;
		break;
	}
}
BaseServerObject::~BaseServerObject()
{
	if(m_containerLock)
		EP_DELETE m_containerLock;
	m_containerLock=NULL;
}
BaseServerObject & BaseServerObject::operator=(const BaseServerObject&b)
{
	if(this!=&b)
	{
		if(m_containerLock)
			EP_DELETE m_containerLock;
		m_containerLock=NULL;


		Thread::operator=(b);
		SmartObject::operator =(b);
		
		m_waitTime=b.m_waitTime;
		m_syncPolicy=b.m_syncPolicy;
		m_container=b.m_container;
		m_lockPolicy=b.m_lockPolicy;
		switch(m_lockPolicy)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_containerLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_containerLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_containerLock=EP_NEW epl::NoLock();
			break;
		default:
			m_containerLock=NULL;
			break;
		}
	}
	return *this;
}

void BaseServerObject::SetWaitTime(unsigned int milliSec)
{
	m_waitTime=milliSec;
}
unsigned int BaseServerObject::GetWaitTime()
{
	return m_waitTime;
}

void BaseServerObject::setContainer(ServerObjectList *container)
{
	LockObj lock(m_containerLock);
	m_container=container;
}
bool BaseServerObject::removeSelfFromContainer()
{
	LockObj lock(m_containerLock);
	bool ret=false;
	if(m_container)
		ret=m_container->Remove(this);
	m_container=NULL;
	return ret;
}

void BaseServerObject::setSyncPolicy(SyncPolicy syncPolicy)
{
	m_syncPolicy=syncPolicy;
}