/*! 
TcpProxyServer for the EpServerEngine
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
#include "epTcpProxyServer.h"
#include "epTcpProxyHandler.h"
#include "epAsyncTcpClient.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;


TcpProxyServer::TcpProxyServer(epl::LockPolicy lockPolicyType):BaseProxyServer(lockPolicyType)
{
	m_proxyServer=EP_NEW AsyncTcpServer(lockPolicyType);


}
TcpProxyServer::TcpProxyServer(const TcpProxyServer& b):BaseProxyServer(b)
{
	m_proxyServer=EP_NEW AsyncTcpServer(*((AsyncTcpServer*)b.m_proxyServer));

}
TcpProxyServer::~TcpProxyServer()
{
}
TcpProxyServer & TcpProxyServer::operator=(const TcpProxyServer&b)
{
	if(this!=&b)
	{
		BaseProxyServer::operator =(b);
	}
	return *this;
}

void TcpProxyServer::OnNewConnection(SocketInterface *socket)
{
	epl::LockObj lock(m_baseProxyServerLock);
	TcpProxyHandler *newHandler=EP_NEW TcpProxyHandler(m_callBack,m_callBack->GetForwardServerInfo(socket->GetSockAddress()),socket);
	m_proxyHandlerList.push_back(newHandler);
}
