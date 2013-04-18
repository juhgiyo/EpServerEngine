/*! 
BaseProxyHandler for the EpServerEngine
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

void BaseProxyHandler::OnReceived(SocketInterface *socket,const Packet&receivedPacket)
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

void BaseProxyHandler::OnReceived(ClientInterface *client,const Packet&receivedPacket)
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