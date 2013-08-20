/*! 
ServerObjectList for the EpServerEngine

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
#include "epServerObjectList.h"
#include "epPacket.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

ServerObjectList::ServerObjectList(unsigned int waitTimeMilliSec, epl::LockPolicy lockPolicyType)
{
	m_waitTime=waitTimeMilliSec;
	m_lockPolicy=lockPolicyType;
	m_serverObjRemover=ServerObjectRemover(waitTimeMilliSec,lockPolicyType);
	m_sizeEvent=EventEx(false,false);
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
}

ServerObjectList::ServerObjectList(const ServerObjectList& b)
{
	m_sizeEvent=b.m_sizeEvent;
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
	m_waitTime=b.m_waitTime;

	ServerObjectList&unSafeB=const_cast<ServerObjectList&>(b);
	unSafeB.m_listLock->Lock();
	m_objectList=b.m_objectList;
	vector<BaseServerObject*>::iterator iter;
	for(iter=m_objectList.begin();iter!=m_objectList.end();iter++)
	{
		(*iter)->RetainObj();
		(*iter)->setContainer(this);

	}
	unSafeB.m_listLock->Unlock();

	m_serverObjRemover=b.m_serverObjRemover;

	
}

ServerObjectList::~ServerObjectList()
{
	resetList();
}

void ServerObjectList::resetList()
{
	Clear();
	if(m_listLock)
		EP_DELETE m_listLock;
	m_listLock=NULL;
}
ServerObjectList & ServerObjectList::operator=(const ServerObjectList&b)
{
	if(this!=&b)
	{
		resetList();

		m_sizeEvent=b.m_sizeEvent;
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
		m_waitTime=b.m_waitTime;
		ServerObjectList&unSafeB=const_cast<ServerObjectList&>(b);
		unSafeB.m_listLock->Lock();
		m_objectList=b.m_objectList;
		vector<BaseServerObject*>::iterator iter;
		for(iter=m_objectList.begin();iter!=m_objectList.end();iter++)
		{
			(*iter)->RetainObj();
			(*iter)->setContainer(this);

		}
		unSafeB.m_listLock->Unlock();

		m_serverObjRemover=b.m_serverObjRemover;

	}
	return *this;
}

void ServerObjectList::SetWaitTime(unsigned int milliSec)
{
	epl::LockObj lock(m_listLock);
	m_waitTime=milliSec;
}
unsigned int ServerObjectList::GetWaitTime()
{
	epl::LockObj lock(m_listLock);
	return m_waitTime;
}


bool ServerObjectList::Remove(const BaseServerObject* serverObj)
{
	epl::LockObj lock(m_listLock);
	for(ssize_t idx=static_cast<ssize_t>(m_objectList.size())-1;idx>=0;idx--)
	{
		if((m_objectList.at(idx))==serverObj)
		{
			m_serverObjRemover.Push(m_objectList.at(idx));
			m_objectList.erase(m_objectList.begin()+idx);
			m_sizeEvent.SetEvent();
			return true;
		}
	}
	return false;
}

void ServerObjectList::Clear()
{
	epl::LockObj lock(m_listLock);
	vector<BaseServerObject*>::iterator iter;
	for(iter=m_objectList.begin();iter!=m_objectList.end();iter++)
	{
		if(*iter)
		{
			(*iter)->setContainer(NULL);
			m_serverObjRemover.Push(*iter);
			
		}
	}
	m_objectList.clear();
	m_sizeEvent.SetEvent();
}

void ServerObjectList::Push(BaseServerObject* obj)
{
	epl::LockObj lock(m_listLock);
	if(obj)
	{
		obj->RetainObj();
		m_objectList.push_back(obj);
		obj->setContainer(this);
	}
	
}

vector<BaseServerObject*> ServerObjectList::GetList() const
{
	epl::LockObj lock(m_listLock);
	return m_objectList;
}

size_t ServerObjectList::Count() const
{
	epl::LockObj lock(m_listLock);
	return m_objectList.size();
}

void ServerObjectList::Do(void (__cdecl *DoFunc)(BaseServerObject*,unsigned int,va_list),unsigned int argCount,...)
{
	m_listLock->Lock();
	vector<BaseServerObject*> objList=m_objectList;
	m_listLock->Unlock();

	void *argPtr=NULL;
	va_list ap=NULL;
	va_start (ap , argCount);         /* Initialize the argument list. */
	for(ssize_t idx=static_cast<ssize_t>(objList.size())-1;idx>=0;idx--)
	{
		DoFunc(objList.at(idx),argCount,ap);
	}

	va_end (ap);                  /* Clean up. */
}

void ServerObjectList::Do(void (__cdecl *DoFunc)(BaseServerObject*,unsigned int,va_list),unsigned int argCount,va_list args)
{
	m_listLock->Lock();
	vector<BaseServerObject*> objList=m_objectList;
	m_listLock->Unlock();

	for(ssize_t idx=static_cast<ssize_t>(objList.size())-1;idx>=0;idx--)
	{
		DoFunc(objList.at(idx),argCount,args);
	}
}


void ServerObjectList::WaitForListSizeDecrease()
{
	m_sizeEvent.WaitForEvent();
}