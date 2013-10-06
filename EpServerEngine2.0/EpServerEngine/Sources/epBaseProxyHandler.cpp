/*! 
BaseProxyHandler for the EpServerEngine

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
#include "epBaseProxyHandler.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;


BaseProxyHandler::BaseProxyHandler(ProxyServerCallbackInterface *callBack, SocketInterface *socket, epl::LockPolicy lockPolicyType)
{
	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_baseProxyHandlerLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_baseProxyHandlerLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_baseProxyHandlerLock=EP_NEW epl::NoLock();
		break;
	default:
		m_baseProxyHandlerLock=NULL;
		break;
	}

	m_callBack=callBack;
	m_client=socket;
	socket->SetCallbackObject(this);

}

BaseProxyHandler::~BaseProxyHandler()
{	
	if(m_forwardClient)
	{
		m_forwardClient->Disconnect();
		EP_DELETE m_forwardClient;
	}
	if(m_baseProxyHandlerLock)
		EP_DELETE m_baseProxyHandlerLock;
}

void BaseProxyHandler::OnReceived(SocketInterface *socket,const Packet*receivedPacket,ReceiveStatus status)
{
	epl::LockObj lock(m_baseProxyHandlerLock);
	m_callBack->OnReceivedFromClient(m_client,m_forwardClient,receivedPacket);
}
void BaseProxyHandler::OnDisconnect(SocketInterface *socket)
{
	if(m_forwardClient)
		m_forwardClient->Disconnect();
	epl::LockObj lock(m_baseProxyHandlerLock);
	m_callBack->OnDisconnect(socket->GetSockAddress());
}

void BaseProxyHandler::OnReceived(ClientInterface *client,const Packet*receivedPacket,ReceiveStatus status)
{
	epl::LockObj lock(m_baseProxyHandlerLock);
	m_callBack->OnReceivedFromForwardServer(m_client,client,receivedPacket);
}

void BaseProxyHandler::OnDisconnect(ClientInterface *client)
{
	m_client->KillConnection();
}


void BaseProxyHandler::SetCallbackObject(ProxyServerCallbackInterface *callBackObj)
{
	epl::LockObj lock(m_baseProxyHandlerLock);
	m_callBack=callBackObj;
}

ProxyServerCallbackInterface *BaseProxyHandler::GetCallbackObject()
{
	epl::LockObj lock(m_baseProxyHandlerLock);
	return m_callBack;
}

epl::EpTString BaseProxyHandler::GetIP() const
{
	if(m_client)
	{
		return m_client->GetIP();
	}
	return _T("");
}

sockaddr BaseProxyHandler::GetSockAddress() const
{
	if(m_client)
	{
		return m_client->GetSockAddress();
	}
	return sockaddr();
}
