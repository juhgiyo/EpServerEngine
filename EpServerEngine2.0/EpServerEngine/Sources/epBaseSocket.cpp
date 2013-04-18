/*! 
BaseSocket for the EpServerEngine
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
#include "epBaseSocket.h"
#include "epAsyncTcpServer.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;
BaseSocket::BaseSocket(ServerCallbackInterface *callBackObj,unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType): BaseServerObject(waitTimeMilliSec,lockPolicyType)
{
	EP_ASSERT(callBackObj);
	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_baseSocketLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_baseSocketLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_baseSocketLock=EP_NEW epl::NoLock();
		break;
	default:
		m_baseSocketLock=NULL;
		break;
	}
	m_callBackObj=callBackObj;
	m_owner=NULL;
}

BaseSocket::~BaseSocket()
{
	if(m_baseSocketLock)
		EP_DELETE m_baseSocketLock;
	m_baseSocketLock=NULL;

	m_owner=NULL;
}



void BaseSocket::SetWaitTime(unsigned int milliSec)
{
	BaseServerObject::SetWaitTime(milliSec);
}
unsigned int BaseSocket::GetWaitTime() const
{
	return BaseServerObject::GetWaitTime();
}

void BaseSocket::setOwner(BaseServerObject * owner )
{
	epl::LockObj lock(m_baseSocketLock);
	m_owner=owner;
}
void BaseSocket::setSockAddr(sockaddr sockAddr)
{
	epl::LockObj lock(m_baseSocketLock);
	m_sockAddr=sockAddr;
}
bool BaseSocket::IsConnectionAlive() const
{
	return (GetStatus()!=Thread::THREAD_STATUS_TERMINATED);
}

sockaddr BaseSocket::GetSockAddress() const
{
	return m_sockAddr;
}

BaseServerObject * BaseSocket::GetOwner()
{
	return m_owner;
}



void BaseSocket::SetCallbackObject(ServerCallbackInterface *callBackObj)
{
	EP_ASSERT(callBackObj);
	epl::LockObj lock(m_baseSocketLock);
	m_callBackObj=callBackObj;
}

ServerCallbackInterface *BaseSocket::GetCallbackObject()
{
	return m_callBackObj;
}
