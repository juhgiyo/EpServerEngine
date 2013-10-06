/*! 
BaseSocket for the EpServerEngine

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


BaseServerObject * BaseSocket::GetOwner()
{
	return m_owner;
}

epl::EpTString BaseSocket::GetIP() const
{
	sockaddr socketAddr=m_sockAddr;
	TCHAR ip[INET6_ADDRSTRLEN] = {0};
	unsigned long ipSize=INET6_ADDRSTRLEN;
	WSAPROTOCOL_INFO protocolInfo;
	WSAAddressToString(&socketAddr,sizeof(sockaddr),&protocolInfo,ip,&ipSize);
	epl::EpTString retString=ip;
	return retString;
}

sockaddr BaseSocket::GetSockAddress() const
{
	return m_sockAddr;
}

epl::EpTString BaseSocket::GetIP(sockaddr socketAddr)
{
	TCHAR ip[INET6_ADDRSTRLEN] = {0};
	unsigned long ipSize=INET6_ADDRSTRLEN;
	WSAPROTOCOL_INFO protocolInfo;
	WSAAddressToString(&socketAddr,sizeof(sockaddr),&protocolInfo,ip,&ipSize);
	epl::EpTString retString=ip;
	return retString;
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
