/*! 
BaseServer for the EpServerEngine
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


using namespace epse;

BaseServer::BaseServer(const TCHAR *  port,epl::BaseCallbackObject *callbackObj, epl::LockPolicy lockPolicyType):BaseServerObject(callbackObj,lockPolicyType)
{
	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_lock=EP_NEW epl::CriticalSectionEx();
		m_disconnectLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_lock=EP_NEW epl::Mutex();
		m_disconnectLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_lock=EP_NEW epl::NoLock();
		m_disconnectLock=EP_NEW epl::NoLock();
		break;
	default:
		m_lock=NULL;
		m_disconnectLock=NULL;
		break;
	}
	SetPort(port);
	m_listenSocket=INVALID_SOCKET;
	m_result=0;
}

BaseServer::BaseServer(const BaseServer& b):BaseServerObject(b)
{
	m_listenSocket=INVALID_SOCKET;
	m_result=0;
	m_port=b.m_port;
	m_lockPolicy=b.m_lockPolicy;
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_lock=EP_NEW epl::CriticalSectionEx();
		m_disconnectLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_lock=EP_NEW epl::Mutex();
		m_disconnectLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_lock=EP_NEW epl::NoLock();
		m_disconnectLock=EP_NEW epl::NoLock();
		break;
	default:
		m_lock=NULL;
		m_disconnectLock=NULL;
		break;
	}
}
BaseServer::~BaseServer()
{
	StopServer();

	if(m_lock)
		EP_DELETE m_lock;
	if(m_disconnectLock)
		EP_DELETE m_disconnectLock;
}

void  BaseServer::SetPort(const TCHAR *  port)
{
	epl::LockObj lock(m_lock);

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
	epl::LockObj lock(m_lock);
	if(!m_port.length())
		return _T("");
#if defined(_UNICODE) || defined(UNICODE)
	epl::EpTString retString=epl::System::MultiByteToWideChar(m_port.c_str());
	return retString;
#else //defined(_UNICODE) || defined(UNICODE)
	return m_port;
#endif //defined(_UNICODE) || defined(UNICODE)
}


vector<BaseServerObject*> BaseServer::GetWorkerList() const
{
	return m_workerList.GetList();
}
void BaseServer::execute()
{
	SOCKET clientSocket;
	while(1)
	{
		clientSocket=accept(m_listenSocket, NULL, NULL);
		if(clientSocket == INVALID_SOCKET)
		{
			break;			
		}
		else
		{
			BaseServerWorker *accWorker=createNewWorker();
			accWorker->Start(reinterpret_cast<void*>(clientSocket));
			m_workerList.Push(accWorker);
			accWorker->ReleaseObj();
		}
		m_workerList.RemoveTerminated();
	}
	stopServer(true);
} 


bool BaseServer::StartServer()
{
	epl::LockObj lock(m_lock);
	if(IsServerStarted())
		return true;
	if(!m_port.length())
	{
		m_port=DEFAULT_PORT;
	}

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

	ZeroMemory(&m_hints, sizeof(m_hints));
	m_hints.ai_family = AF_INET;
	m_hints.ai_socktype = SOCK_STREAM;
	m_hints.ai_protocol = IPPROTO_TCP;
	m_hints.ai_flags = AI_PASSIVE;


	// Resolve the server address and port
	iResult = getaddrinfo(NULL, m_port.c_str(), &m_hints, &m_result);
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

void BaseServer::Broadcast(const Packet& packet)
{
	m_workerList.RemoveTerminated();
	vector<BaseServerObject*>::iterator iter;
	vector<BaseServerObject*> clientList=m_workerList.GetList();
	for(iter=clientList.begin();iter!=clientList.end();iter++)
	{
		((BaseServerWorker*)(*iter))->Send(packet);
	}
}

void BaseServer::ShutdownAllClient()
{
	m_workerList.Clear();
}


bool BaseServer::IsServerStarted() const
{
	return (GetStatus()==Thread::THREAD_STATUS_STARTED);
}

void BaseServer::StopServer()
{
	epl::LockObj lock(m_lock);
	if(!IsServerStarted())
	{
		return;
	}
	stopServer(false);
}

void BaseServer::cleanUpServer()
{
	if(m_result)
	{
		freeaddrinfo(m_result);
		m_result=NULL;
	}
	if(m_listenSocket!=INVALID_SOCKET)
	{
		closesocket(m_listenSocket);
		m_listenSocket=INVALID_SOCKET;
	}
	WSACleanup();

}

void BaseServer::stopServer(bool fromInternal)
{
	if(!m_disconnectLock->TryLock())
	{
		return;
	}
	if(IsServerStarted())
	{
		// No longer need server socket
		if(m_listenSocket!=INVALID_SOCKET)
		{
			closesocket(m_listenSocket);
			m_listenSocket=INVALID_SOCKET;
		}
		if(!fromInternal)
			TerminateAfter(WAITTIME_INIFINITE);
		
		ShutdownAllClient();
	}
	
	cleanUpServer();

	m_disconnectLock->Unlock();
}
