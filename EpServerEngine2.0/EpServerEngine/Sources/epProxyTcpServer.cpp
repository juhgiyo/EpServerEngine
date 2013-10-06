/*! 
ProxyTcpServer for the EpServerEngine

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
#include "epProxyTcpServer.h"
#include "epProxyTcpHandler.h"
#include "epAsyncTcpClient.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;


ProxyTcpServer::ProxyTcpServer(epl::LockPolicy lockPolicyType):BaseProxyServer(lockPolicyType)
{
	m_proxyServer=EP_NEW AsyncTcpServer(lockPolicyType);


}
ProxyTcpServer::ProxyTcpServer(const ProxyTcpServer& b):BaseProxyServer(b)
{
	m_proxyServer=EP_NEW AsyncTcpServer(*((AsyncTcpServer*)b.m_proxyServer));

}
ProxyTcpServer::~ProxyTcpServer()
{
}
ProxyTcpServer & ProxyTcpServer::operator=(const ProxyTcpServer&b)
{
	if(this!=&b)
	{
		BaseProxyServer::operator =(b);
	}
	return *this;
}

void ProxyTcpServer::OnNewConnection(SocketInterface *socket)
{
	epl::LockObj lock(m_baseProxyServerLock);
	ProxyTcpHandler *newHandler=EP_NEW ProxyTcpHandler(m_callBack,m_callBack->GetForwardServerInfo(socket->GetSockAddr()),socket);
	m_proxyHandlerList.push_back(newHandler);
}
