/*! 
ServerObjectList for the EpServerEngine
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
#include "epServerObjectList.h"
#include "epPacket.h"
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
	for(iter=m_objectList.begin();iter!=m_objectList.end();)
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
		for(iter=m_objectList.begin();iter!=m_objectList.end();)
		{
			(*iter)->RetainObj();

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
	for(int idx=m_objectList.size()-1;idx>=0;idx--)
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
			m_serverObjRemover.Push(*iter);
			(*iter)->setContainer(NULL);
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

unsigned int ServerObjectList::Count() const
{
	epl::LockObj lock(m_listLock);
	return m_objectList.size();
}

void ServerObjectList::Do(void (__cdecl *DoFunc)(BaseServerObject*,unsigned int,va_list),unsigned int argCount,...)
{
	epl::LockObj lock(m_listLock);

	void *argPtr=NULL;
	va_list ap=NULL;
	va_start (ap , argCount);         /* Initialize the argument list. */
	for(int idx=m_objectList.size()-1;idx>=0;idx--)
	{
		DoFunc(m_objectList.at(idx),argCount,ap);
	}

	va_end (ap);                  /* Clean up. */
}

void ServerObjectList::Do(void (__cdecl *DoFunc)(BaseServerObject*,unsigned int,va_list),unsigned int argCount,va_list args)
{
	epl::LockObj lock(m_listLock);

	for(int idx=m_objectList.size()-1;idx>=0;idx--)
	{
		DoFunc(m_objectList.at(idx),argCount,args);
	}
}


void ServerObjectList::WaitForListSizeDecrease()
{
	m_sizeEvent.WaitForEvent();
}