/*! 
BaseServer for the BaseServer
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
#include "epBaseServer.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

BaseServer::BaseServer(epl::LockPolicy lockPolicyType):BaseServerObject(WAITTIME_INIFINITE,lockPolicyType)
{
	m_socketList=ServerObjectList(WAITTIME_INIFINITE,lockPolicyType);
	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_baseServerLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_baseServerLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_baseServerLock=EP_NEW epl::NoLock();
		break;
	default:
		m_baseServerLock=NULL;
		break;
	}
	m_listenSocket=INVALID_SOCKET;
	m_result=0;
	m_maxConnectionCount=CONNECTION_LIMIT_INFINITE;
	SetPort(_T(DEFAULT_PORT));
	m_callBackObj=NULL;
}

BaseServer::BaseServer(const BaseServer& b):BaseServerObject(b)
{
	m_listenSocket=INVALID_SOCKET;
	m_result=0;

	m_lockPolicy=b.m_lockPolicy;
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_baseServerLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_baseServerLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_baseServerLock=EP_NEW epl::NoLock();
		break;
	default:
		m_baseServerLock=NULL;
		break;
	}

	LockObj lock(b.m_baseServerLock);
	m_port=b.m_port;
	m_maxConnectionCount=b.m_maxConnectionCount;
	m_socketList=b.m_socketList;
	m_callBackObj=b.m_callBackObj;
}
BaseServer::~BaseServer()
{
	resetServer();
}

BaseServer & BaseServer::operator=(const BaseServer&b)
{
	if(this!=&b)
	{
		resetServer();

		BaseServerObject::operator =(b);

		m_listenSocket=INVALID_SOCKET;
		m_result=0;

		m_lockPolicy=b.m_lockPolicy;
		switch(m_lockPolicy)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_baseServerLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_baseServerLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_baseServerLock=EP_NEW epl::NoLock();
			break;
		default:
			m_baseServerLock=NULL;
			break;
		}

		LockObj lock(b.m_baseServerLock);
		m_port=b.m_port;
		m_maxConnectionCount=b.m_maxConnectionCount;
		m_socketList=b.m_socketList;
		m_callBackObj=b.m_callBackObj;
	}
	return *this;
}

void BaseServer::resetServer()
{
	StopServer();

	if(m_baseServerLock)
		EP_DELETE m_baseServerLock;
	m_baseServerLock=NULL;
}

void  BaseServer::SetPort(const TCHAR *  port)
{
	epl::LockObj lock(m_baseServerLock);
	setPort(port);
}

void  BaseServer::setPort(const TCHAR *  port)
{
	unsigned int strLength=epl::System::TcsLen(port);
	if(strLength==0)
		m_port=DEFAULT_PORT;
	else
	{
#if defined(_UNICODE) || defined(UNICODE)
		m_port=epl::System::WideCharToMultiByte(port);
#else// defined(_UNICODE) || defined(UNICODE)
		m_port=port;
#endif// defined(_UNICODE) || defined(UNICODE)
	}
}

epl::EpTString BaseServer::GetPort() const
{
	epl::LockObj lock(m_baseServerLock);
	if(!m_port.length())
		return _T("");
#if defined(_UNICODE) || defined(UNICODE)
	epl::EpTString retString=epl::System::MultiByteToWideChar(m_port.c_str());
	return retString;
#else //defined(_UNICODE) || defined(UNICODE)
	return m_port;
#endif //defined(_UNICODE) || defined(UNICODE)
}

void BaseServer::SetMaximumConnectionCount(unsigned int maxConnectionCount)
{
	epl::LockObj lock(m_baseServerLock);
	m_maxConnectionCount=maxConnectionCount;

}
unsigned int BaseServer::GetMaximumConnectionCount() const
{
	epl::LockObj lock(m_baseServerLock);
	return m_maxConnectionCount;
}
void BaseServer::SetCallbackObject(ServerCallbackInterface *callBackObj)
{
	EP_ASSERT(callBackObj);
	m_callBackObj=callBackObj;
}

ServerCallbackInterface *BaseServer::GetCallbackObject()
{
	return m_callBackObj;
}

void BaseServer::SetWaitTime(unsigned int milliSec)
{
	m_waitTime=milliSec;
	m_socketList.SetWaitTime(milliSec);
}

unsigned int BaseServer::GetWaitTime() const
{
	return BaseServerObject::GetWaitTime();
}


void BaseServer::killConnection(BaseServerObject *clientObj,unsigned int argCount,va_list args)
{
	((SocketInterface*)(clientObj))->KillConnection();
}

void BaseServer::ShutdownAllClient()
{
	epl::LockObj lock(m_baseServerLock);
	shutdownAllClient();
}

void BaseServer::shutdownAllClient()
{
	m_socketList.Do(killConnection,0);
	m_socketList.Clear();
}

bool BaseServer::IsServerStarted() const
{
	//return (GetStatus()==Thread::THREAD_STATUS_STARTED);
	return (m_listenSocket!=INVALID_SOCKET);
}

void BaseServer::cleanUpServer()
{
	if(m_listenSocket!=INVALID_SOCKET)
	{
		closesocket(m_listenSocket);
		m_listenSocket=INVALID_SOCKET;
	}
	if(m_result)
	{
		freeaddrinfo(m_result);
		m_result=NULL;
	}

	WSACleanup();

}

void BaseServer::stopServer()
{
	if(IsServerStarted())
	{
		// No longer need server socket
		shutdownAllClient();
	}

	cleanUpServer();
}

