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

using namespace epse;

ServerObjectList::ServerObjectList(epl::LockPolicy lockPolicyType)
{
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
}

ServerObjectList::ServerObjectList(const ServerObjectList& b)
{
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
	m_serverObjRemover=b.m_serverObjRemover;
	vector<BaseServerObject*>::const_iterator iter;
	for(iter=b.m_objectList.begin();iter!=b.m_objectList.end();iter++)
	{
		(*iter)->RetainObj();
		m_objectList.push_back(*iter);
	}
}

ServerObjectList::~ServerObjectList()
{
	Clear();
	m_serverObjRemover.Clear();
	if(m_listLock)
		EP_DELETE m_listLock;
}
ServerObjectList & ServerObjectList::operator=(const ServerObjectList&b)
{
	if(this!=&b)
	{
		Clear();
		epl::LockObj lock(m_listLock);
		m_objectList=b.m_objectList;
		ServerObjectList & unSafeB=const_cast<ServerObjectList&>(b);
		unSafeB.m_objectList.clear();
	}
	return *this;
}

void ServerObjectList::RemoveTerminated()
{
	epl::LockObj lock(m_listLock);
	vector<BaseServerObject*>::iterator iter;
	for(iter=m_objectList.begin();iter!=m_objectList.end();)
	{
		if((*iter)->GetStatus()==epl::Thread::THREAD_STATUS_TERMINATED)
		{
			m_serverObjRemover.Push(*iter);
			iter=m_objectList.erase(iter);
		}
		else
			iter++;

	}
}

void ServerObjectList::Clear()
{
	epl::LockObj lock(m_listLock);
	vector<BaseServerObject*>::iterator iter;
	for(iter=m_objectList.begin();iter!=m_objectList.end();iter++)
	{
		if(*iter)
			m_serverObjRemover.Push(*iter);
	}
	m_objectList.clear();
}

void ServerObjectList::Push(BaseServerObject* obj)
{
	epl::LockObj lock(m_listLock);
	if(obj)
	{
		obj->RetainObj();
		m_objectList.push_back(obj);
	}
	
}

vector<BaseServerObject*> ServerObjectList::GetList() const
{
	epl::LockObj lock(m_listLock);
	return m_objectList;
}