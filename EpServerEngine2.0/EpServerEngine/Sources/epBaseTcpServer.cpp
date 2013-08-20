/*! 
BaseTcpServer for the BaseTcpServer

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
#include "epBaseTcpServer.h"


#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

BaseTcpServer::BaseTcpServer(epl::LockPolicy lockPolicyType):BaseServer(lockPolicyType)
{
}


BaseTcpServer::BaseTcpServer(const BaseTcpServer& b):BaseServer(b)
{}

BaseTcpServer::~BaseTcpServer()
{
}

BaseTcpServer & BaseTcpServer::operator=(const BaseTcpServer&b)
{
	if(this!=&b)
	{
		BaseServer::operator =(b);
	}
	return *this;
}


bool BaseTcpServer::StartServer(const ServerOps &ops)
{
	epl::LockObj lock(m_baseServerLock);
	if(IsServerStarted())
		return true;


	if(ops.callBackObj)
		m_callBackObj=ops.callBackObj;
	EP_ASSERT(m_callBackObj);

	if(ops.port)
	{
		setPort(ops.port);
	}

	if(!m_port.length())
	{
		m_port=DEFAULT_PORT;
	}

	SetWaitTime(ops.waitTimeMilliSec);
	m_maxConnectionCount=ops.maximumConnectionCount;
	
	WSADATA wsaData;
	int iResult;

	m_listenSocket= INVALID_SOCKET;

	m_result = NULL;


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {

		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) WSAStartup failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		return false;
	}
	/// internal use variable2
	struct addrinfo iHints;
	ZeroMemory(&iHints, sizeof(iHints));
	iHints.ai_family = AF_INET;
	iHints.ai_socktype = SOCK_STREAM;
	iHints.ai_protocol = IPPROTO_TCP;
	iHints.ai_flags = AI_PASSIVE;


	// Resolve the server address and port
	iResult = getaddrinfo(NULL, m_port.c_str(), &iHints, &m_result);
	if ( iResult != 0 ) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) getaddrinfo failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);		
		WSACleanup();
		return false;
	}

	// Create a SOCKET for connecting to server
	m_listenSocket = socket(m_result->ai_family, m_result->ai_socktype, m_result->ai_protocol);
	if (m_listenSocket == INVALID_SOCKET) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) socket failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		cleanUpServer();
		return false;
	}

	// set SO_REUSEADDR for setsockopt function to reuse the port immediately as soon as the service exits.
	int sockoptval = 1;
	setsockopt(m_listenSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&sockoptval), sizeof(int));

	// Setup the TCP listening socket
	iResult = bind( m_listenSocket, m_result->ai_addr, static_cast<int>(m_result->ai_addrlen));
	if (iResult == SOCKET_ERROR) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) bind failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		cleanUpServer();
		return false;
	}

	iResult = listen(m_listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) listen failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		cleanUpServer();
		return false;
	}

	// Create thread 1.
	if(Start())
	{
		return true;
	}
	cleanUpServer();
	return false;


}

void BaseTcpServer::StopServer()
{
	epl::LockObj lock(m_baseServerLock);
	if(!IsServerStarted())
	{
		return;
	}
	// No longer need server socket
	if(m_listenSocket!=INVALID_SOCKET)
	{
		closesocket(m_listenSocket);
		m_listenSocket=INVALID_SOCKET;
	}
	TerminateAfter(m_waitTime);
	shutdownAllClient();
	cleanUpServer();
}

