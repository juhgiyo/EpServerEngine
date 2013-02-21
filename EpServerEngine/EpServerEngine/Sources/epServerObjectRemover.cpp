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
}
ServerObjectRemover::ServerObjectRemover(const ServerObjectRemover& b):ServerObjectList(b),Thread(b),SmartObject(b)
{
	m_shouldTerminate=false;
	m_waitTime=b.m_waitTime;
}
ServerObjectRemover::~ServerObjectRemover()
{
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
			Sleep(0);
			continue;
		}
		BaseServerObject* serverObj=m_objectList.front();
		m_objectList.pop();
		m_listLock->Unlock();
		serverObj->ReleaseObj();
	}
}
