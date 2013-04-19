/*! 
BaseClient for the EpServerEngine
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
#include "epBaseClient.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

BaseClient::BaseClient( epl::LockPolicy lockPolicyType) :BaseServerObject(WAITTIME_INIFINITE,lockPolicyType)
{

	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_sendLock=EP_NEW epl::CriticalSectionEx();
		m_generalLock=EP_NEW epl::CriticalSectionEx();
		m_socketLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_sendLock=EP_NEW epl::Mutex();
		m_generalLock=EP_NEW epl::Mutex();
		m_socketLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_sendLock=EP_NEW epl::NoLock();
		m_generalLock=EP_NEW epl::NoLock();
		m_socketLock=EP_NEW epl::NoLock();
		break;
	default:
		m_sendLock=NULL;
		m_generalLock=NULL;
		m_socketLock=NULL;
		break;
	}
	m_connectSocket=INVALID_SOCKET;
	m_result=0;

	setHostName(_T(DEFAULT_HOSTNAME));
	setPort(_T(DEFAULT_PORT));
	m_callBackObj=NULL;
}

BaseClient::BaseClient(const BaseClient& b) :BaseServerObject(b)
{


	m_lockPolicy=b.m_lockPolicy;
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_sendLock=EP_NEW epl::CriticalSectionEx();
		m_generalLock=EP_NEW epl::CriticalSectionEx();
		m_socketLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_sendLock=EP_NEW epl::Mutex();
		m_generalLock=EP_NEW epl::Mutex();
		m_socketLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_sendLock=EP_NEW epl::NoLock();
		m_generalLock=EP_NEW epl::NoLock();
		m_socketLock=EP_NEW epl::NoLock();
		break;
	default:
		m_sendLock=NULL;
		m_generalLock=NULL;
		m_socketLock=NULL;
		break;
	}
	m_connectSocket=INVALID_SOCKET;
	m_result=0;

	LockObj lock(b.m_generalLock);
	m_hostName=b.m_hostName;
	m_port=b.m_port;
	m_callBackObj=b.m_callBackObj;

}
BaseClient::~BaseClient()
{
	resetClient();
}

BaseClient & BaseClient::operator=(const BaseClient&b)
{
	if(this!=&b)
	{
		resetClient();

		BaseServerObject::operator =(b);

		m_lockPolicy=b.m_lockPolicy;
		switch(m_lockPolicy)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_sendLock=EP_NEW epl::CriticalSectionEx();
			m_generalLock=EP_NEW epl::CriticalSectionEx();
			m_socketLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_sendLock=EP_NEW epl::Mutex();
			m_generalLock=EP_NEW epl::Mutex();
			m_socketLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_sendLock=EP_NEW epl::NoLock();
			m_generalLock=EP_NEW epl::NoLock();
			m_socketLock=EP_NEW epl::NoLock();
			break;
		default:
			m_sendLock=NULL;
			m_generalLock=NULL;
			m_socketLock=NULL;
			break;
		}
		m_connectSocket=INVALID_SOCKET;
		m_result=0;

		LockObj lock(b.m_generalLock);
		m_hostName=b.m_hostName;
		m_port=b.m_port;
		m_callBackObj=b.m_callBackObj;
	}
	return *this;
}

void BaseClient::resetClient()
{
	Disconnect();

	if(m_sendLock)
		EP_DELETE m_sendLock;
	m_sendLock=NULL;
	if(m_generalLock)
		EP_DELETE m_generalLock;
	m_generalLock=NULL;
}

void  BaseClient::SetHostName(const TCHAR * hostName)
{
	epl::LockObj lock(m_generalLock);
	setHostName(hostName);

}

void  BaseClient::SetPort(const TCHAR *port)
{
	epl::LockObj lock(m_generalLock);
	setPort(port);
}


SOCKET BaseClient::getSocket()
{
	epl::LockObj lock(m_socketLock);
	return m_connectSocket;
}
void BaseClient::setSocket(SOCKET sock)
{
	epl::LockObj lock(m_socketLock);
	m_connectSocket=sock;
}
void BaseClient::setHostName(const TCHAR * hostName)
{
	unsigned int strLength=epl::System::TcsLen(hostName);
	if(strLength==0)
		m_hostName=DEFAULT_HOSTNAME;
	else
	{		
#if defined(_UNICODE) || defined(UNICODE)
		m_hostName=epl::System::WideCharToMultiByte(hostName);
#else// defined(_UNICODE) || defined(UNICODE)
		m_hostName=hostName;
#endif// defined(_UNICODE) || defined(UNICODE)
	}
}

void BaseClient::setPort(const TCHAR *port)
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


epl::EpTString BaseClient::GetHostName() const
{
	epl::LockObj lock(m_generalLock);
	if(!m_hostName.length())
		return _T("");

#if defined(_UNICODE) || defined(UNICODE)
	epl::EpTString retString=epl::System::MultiByteToWideChar(m_hostName.c_str());
	return retString;
#else //defined(_UNICODE) || defined(UNICODE)
	return m_hostName;
#endif //defined(_UNICODE) || defined(UNICODE)

}
epl::EpTString BaseClient::GetPort() const
{
	epl::LockObj lock(m_generalLock);
	if(!m_port.length())
		return _T("");

#if defined(_UNICODE) || defined(UNICODE)
	epl::EpTString retString=epl::System::MultiByteToWideChar(m_port.c_str());;
	return retString;
#else //defined(_UNICODE) || defined(UNICODE)
	return m_port;
#endif //defined(_UNICODE) || defined(UNICODE)

}



void BaseClient::SetCallbackObject(ClientCallbackInterface *callBackObj)
{
	EP_ASSERT(callBackObj);
	m_callBackObj=callBackObj;
}
ClientCallbackInterface *BaseClient::GetCallbackObject()
{
	return m_callBackObj;
}


void BaseClient::SetWaitTime(unsigned int milliSec)
{
	BaseServerObject::SetWaitTime(milliSec);
}
		
unsigned int BaseClient::GetWaitTime() const
{
	return BaseServerObject::GetWaitTime();
}

bool BaseClient::IsConnectionAlive() const
{
	return (GetStatus()==Thread::THREAD_STATUS_STARTED);
}

void BaseClient::cleanUpClient()
{
	SOCKET connectSocket=getSocket();
	setSocket(INVALID_SOCKET);
	if(connectSocket!=INVALID_SOCKET)
	{
		closesocket(connectSocket);
	}
	if(m_result)
	{
		freeaddrinfo(m_result);
		m_result=NULL;
	}
	WSACleanup();

}




