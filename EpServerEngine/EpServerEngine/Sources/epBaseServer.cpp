/*! 
BaseServer for the EpServerEngine

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
#include "epBaseServer.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

BaseServer::BaseServer(const TCHAR *  port,SyncPolicy syncPolicy, unsigned int maximumConnectionCount,unsigned int waitTimeMilliSec, epl::LockPolicy lockPolicyType):BaseServerObject(waitTimeMilliSec,lockPolicyType)
{
	m_workerList=ServerObjectList(waitTimeMilliSec,lockPolicyType);
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
	SetPort(port);
	m_listenSocket=INVALID_SOCKET;
	m_result=0;
	setSyncPolicy(syncPolicy);
	m_maxConnectionCount=maximumConnectionCount;
	m_parserList=NULL;
	memset(&m_socketInfo,0,sizeof(sockaddr));
	m_sockAddrSize=sizeof(sockaddr);
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
	m_workerList=b.m_workerList;
	m_parserList=b.m_parserList;
	if(m_parserList)
		m_parserList->RetainObj();	
	memset(&m_socketInfo,0,sizeof(sockaddr));
	m_sockAddrSize=sizeof(sockaddr);
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
		m_workerList=b.m_workerList;
		m_parserList=b.m_parserList;
		if(m_parserList)
			m_parserList->RetainObj();
		memset(&m_socketInfo,0,sizeof(sockaddr));
		m_sockAddrSize=sizeof(sockaddr);
	}
	return *this;
}

void BaseServer::resetServer()
{
	StopServer();

	if(m_parserList)
		m_parserList->ReleaseObj();
	m_parserList=NULL;

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
	if(port==NULL)
	{
		m_port=DEFAULT_PORT;
		return;
	}
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

bool BaseServer::SetSyncPolicy(SyncPolicy syncPolicy)
{
	if(IsServerStarted())
		return false;
	epl::LockObj lock(m_baseServerLock);
	setSyncPolicy(syncPolicy);
	return true;
}

SyncPolicy BaseServer::GetSyncPolicy() const
{
	epl::LockObj lock(m_baseServerLock);
	return m_syncPolicy;
}

vector<BaseServerObject*> BaseServer::GetWorkerList() const
{
	return m_workerList.GetList();
}

void BaseServer::execute()
{
	SOCKET clientSocket;

	if(m_syncPolicy==SYNC_POLICY_SYNCHRONOUS)
	{
		if(m_parserList==NULL)
		{
			m_parserList=EP_NEW ParserList(m_syncPolicy,m_waitTime,m_lockPolicy);
			if(!m_parserList || !m_parserList->StartParse())
			{
				epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Unable to start to Global Parser!\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
				if(m_parserList)
					m_parserList->ReleaseObj();
				m_parserList=NULL;
				stopServer();
				return;
			}
		}
	}

	while(1)
	{
		memset(&m_socketInfo,0,sizeof(sockaddr));
		m_sockAddrSize=sizeof(sockaddr);
		clientSocket=accept(m_listenSocket,&m_socketInfo,&m_sockAddrSize);
		if(clientSocket == INVALID_SOCKET || m_listenSocket== INVALID_SOCKET)
		{
			break;			
		}
		else
		{
			BaseServerWorker *accWorker=createNewWorker(m_socketInfo);
			if(!accWorker)
			{
				closesocket(clientSocket);
				continue;
			}
			accWorker->setSyncPolicy(m_syncPolicy);
			if(m_syncPolicy==SYNC_POLICY_SYNCHRONOUS)
				accWorker->setParserList(m_parserList);
			accWorker->setClientSocket(clientSocket);
			accWorker->setOwner(this);
			m_workerList.Push(accWorker);
			accWorker->Start();
			accWorker->ReleaseObj();
			if(GetMaximumConnectionCount()!=CONNECTION_LIMIT_INFINITE)
			{
				while(m_workerList.Count()>=GetMaximumConnectionCount())
				{
					m_workerList.WaitForListSizeDecrease();
				}
			}
			
		}
	}
	
	stopServer();
} 


bool BaseServer::StartServer(const TCHAR * port)
{
	epl::LockObj lock(m_baseServerLock);
	if(IsServerStarted())
		return true;

	if(port)
	{
		setPort(port);
	}

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

void BaseServer::sendPacket(BaseServerObject *clientObj,unsigned int argCount,va_list args)
{
	void *argPtr=NULL;
	unsigned int waitTime=WAITTIME_INIFINITE;
	EP_ASSERT(argCount);
	argPtr = va_arg (args, void *);
	waitTime=va_arg(args,unsigned int);

	Packet *packetPtr=(Packet*)argPtr;
	((BaseServerWorker*)(clientObj))->Send(*packetPtr,waitTime);
}

void BaseServer::killConnection(BaseServerObject *clientObj,unsigned int argCount,va_list args)
{
	((BaseServerWorker*)(clientObj))->KillConnection();
}

void BaseServer::Broadcast(const Packet& packet, unsigned int waitTimeInMilliSec)
{
	m_workerList.Do(sendPacket,2,&packet,waitTimeInMilliSec);
}
void BaseServer::CommandWorkers(void (__cdecl *DoFunc)(BaseServerObject*,unsigned int,va_list),unsigned int argCount,...)
{
	void *argPtr=NULL;
	va_list ap=NULL;
	va_start (ap , argCount);         /* Initialize the argument list. */
	m_workerList.Do(DoFunc,argCount,ap);
	va_end (ap);                  /* Clean up. */
}
void BaseServer::ShutdownAllClient()
{
	epl::LockObj lock(m_baseServerLock);
	shutdownAllClient();
}

void BaseServer::shutdownAllClient()
{
	m_workerList.Do(killConnection,0);
	m_workerList.Clear();
}

bool BaseServer::IsServerStarted() const
{
	return (GetStatus()==Thread::THREAD_STATUS_STARTED);
}

void BaseServer::StopServer()
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
	if(m_syncPolicy==SYNC_POLICY_SYNCHRONOUS)
	{
		if(m_parserList)
		{
			m_parserList->StopParse();
			m_parserList->Clear();
			m_parserList->ReleaseObj();
			m_parserList=NULL;
		}
	}		
	shutdownAllClient();
	cleanUpServer();
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
// 		if(m_listenSocket!=INVALID_SOCKET)
// 		{
// 			closesocket(m_listenSocket);
// 			m_listenSocket=INVALID_SOCKET;
// 		}
		if(m_syncPolicy==SYNC_POLICY_SYNCHRONOUS)
		{
			if(m_parserList)
			{
				m_parserList->StopParse();
				m_parserList->Clear();
				m_parserList->ReleaseObj();
				m_parserList=NULL;
			}
		}		
		shutdownAllClient();
	}
	
	cleanUpServer();
}

