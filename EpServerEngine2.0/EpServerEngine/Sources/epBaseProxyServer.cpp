/*! 
BaseProxyServer for the EpServerEngine

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
#include "epBaseProxyServer.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;


BaseProxyServer::BaseProxyServer(epl::LockPolicy lockPolicyType)
{
	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_baseProxyServerLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_baseProxyServerLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_baseProxyServerLock=EP_NEW epl::NoLock();
		break;
	default:
		m_baseProxyServerLock=NULL;
		break;
	}
}
BaseProxyServer::BaseProxyServer(const BaseProxyServer& b)
{
	m_lockPolicy=b.m_lockPolicy;
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_baseProxyServerLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_baseProxyServerLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_baseProxyServerLock=EP_NEW epl::NoLock();
		break;
	default:
		m_baseProxyServerLock=NULL;
		break;
	}
}
BaseProxyServer::~BaseProxyServer()
{
	if(m_proxyServer)
	{
		m_proxyServer->StopServer();
		EP_DELETE m_proxyServer;
	}
	m_baseProxyServerLock->Lock();
	vector<BaseProxyHandler*>::iterator iter;
	for(iter=m_proxyHandlerList.begin();iter!=m_proxyHandlerList.end();iter++)
	{
		EP_DELETE (*iter);
	}
	m_proxyHandlerList.clear();
	m_baseProxyServerLock->Unlock();

	if(m_baseProxyServerLock)
		EP_DELETE m_baseProxyServerLock;


}
BaseProxyServer & BaseProxyServer::operator=(const BaseProxyServer&b)
{
	if(this!=&b)
	{
		if(m_baseProxyServerLock)
			EP_DELETE m_baseProxyServerLock;

		m_lockPolicy=b.m_lockPolicy;
		switch(m_lockPolicy)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_baseProxyServerLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_baseProxyServerLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_baseProxyServerLock=EP_NEW epl::NoLock();
			break;
		default:
			m_baseProxyServerLock=NULL;
			break;
		}
		*m_proxyServer=*b.m_proxyServer;
	}
	return *this;
}
void BaseProxyServer::SetPort(const TCHAR *  port)
{
	if(m_proxyServer)
		m_proxyServer->SetPort(port);
}
epl::EpTString BaseProxyServer::GetPort() const
{
	if(m_proxyServer)
		return m_proxyServer->GetPort();
	return _T("");
}
void BaseProxyServer::SetMaximumConnectionCount(unsigned int maxConnectionCount)
{
	if(m_proxyServer)
		m_proxyServer->SetMaximumConnectionCount(maxConnectionCount);
}
unsigned int BaseProxyServer::GetMaximumConnectionCount() const
{
	if(m_proxyServer)
		return m_proxyServer->GetMaximumConnectionCount();
	return 0;
}
bool BaseProxyServer::StartServer(const ProxyServerOps &ops)
{
	m_baseProxyServerLock->Lock();
	m_callBack=ops.callBackObj;
	EP_ASSERT(m_callBack);
	m_baseProxyServerLock->Unlock();
	ServerOps serverOps;
	serverOps.callBackObj=this;
	serverOps.isAsynchronousReceive=false;
	serverOps.maximumConnectionCount=ops.maximumConnectionCount;
	serverOps.port=ops.port;
	serverOps.waitTimeMilliSec=WAITTIME_INIFINITE;
	if(m_proxyServer)
		return m_proxyServer->StartServer(serverOps);
	return false;
}
void BaseProxyServer::StopServer()
{
	if(m_proxyServer)
		m_proxyServer->StopServer();

	epl::LockObj lock(m_baseProxyServerLock);
	vector<BaseProxyHandler*>::iterator iter;
	for(iter=m_proxyHandlerList.begin();iter!=m_proxyHandlerList.end();iter++)
	{
		EP_DELETE (*iter);
	}
	m_proxyHandlerList.clear();
}
bool BaseProxyServer::IsServerStarted() const
{
	if(m_proxyServer)
		return m_proxyServer->IsServerStarted();
	return false;
}
void BaseProxyServer::ShutdownAllClient()
{
	if(m_proxyServer)
		m_proxyServer->ShutdownAllClient();

	epl::LockObj lock(m_baseProxyServerLock);
	vector<BaseProxyHandler*>::iterator iter;
	for(iter=m_proxyHandlerList.begin();iter!=m_proxyHandlerList.end();iter++)
	{
		EP_DELETE (*iter);
	}
	m_proxyHandlerList.clear();
}
bool BaseProxyServer::OnAccept(sockaddr sockAddr)
{
	epl::LockObj lock(m_baseProxyServerLock);
	return m_callBack->OnAccept(sockAddr);
}

void BaseProxyServer::SetCallbackObject(ProxyServerCallbackInterface *callBackObj)
{
	epl::LockObj lock(m_baseProxyServerLock);
	m_callBack=callBackObj;
	vector<BaseProxyHandler*>::iterator iter;
	for(iter=m_proxyHandlerList.begin();iter!=m_proxyHandlerList.end();iter++)
	{
		if(*iter)
			(*iter)->SetCallbackObject(callBackObj);
	}
}

ProxyServerCallbackInterface *BaseProxyServer::GetCallbackObject()
{
	epl::LockObj lock(m_baseProxyServerLock);
	return m_callBack;
}