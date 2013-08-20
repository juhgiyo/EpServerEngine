/*! 
BaseServerUDP for the EpServerEngine

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
#include "epBaseServerUDP.h"


#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

BaseServerUDP::BaseServerUDP(const TCHAR *  port,SyncPolicy syncPolicy, unsigned int maximumConnectionCount,unsigned int waitTimeMilliSec, epl::LockPolicy lockPolicyType): BaseServerObject(waitTimeMilliSec,lockPolicyType)
{
	m_workerList=ServerObjectList(waitTimeMilliSec,lockPolicyType);
	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_baseServerLock=EP_NEW epl::CriticalSectionEx();
		m_sendLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_baseServerLock=EP_NEW epl::Mutex();
		m_sendLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_baseServerLock=EP_NEW epl::NoLock();
		m_sendLock=EP_NEW epl::NoLock();
		break;
	default:
		m_baseServerLock=NULL;
		m_sendLock=NULL;
		break;
	}
	SetPort(port);
	m_listenSocket=INVALID_SOCKET;
	m_result=0;
	m_maxPacketSize=0;
	setSyncPolicy(syncPolicy);
	m_maxConnectionCount=maximumConnectionCount;
	m_parserList=NULL;
}

BaseServerUDP::BaseServerUDP(const BaseServerUDP& b):BaseServerObject(b)
{
	m_listenSocket=INVALID_SOCKET;
	m_result=0;
	
	m_lockPolicy=b.m_lockPolicy;
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_baseServerLock=EP_NEW epl::CriticalSectionEx();
		m_sendLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_baseServerLock=EP_NEW epl::Mutex();
		m_sendLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_baseServerLock=EP_NEW epl::NoLock();
		m_sendLock=EP_NEW epl::NoLock();
		break;
	default:
		m_baseServerLock=NULL;
		m_sendLock=NULL;
		break;
	}

	LockObj lock(b.m_baseServerLock);
	m_maxPacketSize=b.m_maxPacketSize;
	m_port=b.m_port;
	m_maxConnectionCount=b.m_maxConnectionCount;
	m_workerList=b.m_workerList;
	m_parserList=b.m_parserList;
	if(m_parserList)
		m_parserList->RetainObj();
}
BaseServerUDP::~BaseServerUDP()
{
	resetServer();
}
BaseServerUDP & BaseServerUDP::operator=(const BaseServerUDP&b)
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
			m_sendLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_baseServerLock=EP_NEW epl::Mutex();
			m_sendLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_baseServerLock=EP_NEW epl::NoLock();
			m_sendLock=EP_NEW epl::NoLock();
			break;
		default:
			m_baseServerLock=NULL;
			m_sendLock=NULL;
			break;
		}

		LockObj lock(b.m_baseServerLock);
		m_maxPacketSize=b.m_maxPacketSize;
		m_port=b.m_port;
		m_maxConnectionCount=b.m_maxConnectionCount;
		m_workerList=b.m_workerList;
		m_parserList=b.m_parserList;
		if(m_parserList)
			m_parserList->RetainObj();
	}
	return *this;
}

void BaseServerUDP::resetServer()
{
	StopServer();

	if(m_parserList)
		m_parserList->ReleaseObj();
	m_parserList=NULL;

	if(m_sendLock)
		EP_DELETE m_sendLock;
	m_sendLock=NULL;

	if(m_baseServerLock)
		EP_DELETE m_baseServerLock;
	m_baseServerLock=NULL;

}

void  BaseServerUDP::SetPort(const TCHAR *  port)
{
	epl::LockObj lock(m_baseServerLock);
	setPort(port);
}

void  BaseServerUDP::setPort(const TCHAR *  port)
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


epl::EpTString BaseServerUDP::GetPort() const
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


void BaseServerUDP::SetMaximumConnectionCount(unsigned int maxConnectionCount)
{
	epl::LockObj lock(m_baseServerLock);
	m_maxConnectionCount=maxConnectionCount;

}
unsigned int BaseServerUDP::GetMaximumConnectionCount() const
{
	epl::LockObj lock(m_baseServerLock);
	return m_maxConnectionCount;
}

unsigned int BaseServerUDP::GetMaxPacketByteSize() const
{
	return m_maxPacketSize;
}

int BaseServerUDP::send(const Packet &packet,const sockaddr &clientSockAddr, unsigned int waitTimeInMilliSec)
{

	epl::LockObj lock(m_sendLock);

	// select routine
	TIMEVAL	timeOutVal;
	fd_set	fdSet;
	int		retfdNum = 0;

	FD_ZERO(&fdSet);
	FD_SET(m_listenSocket, &fdSet);
	if(waitTimeInMilliSec!=WAITTIME_INIFINITE)
	{
		// socket select time out setting
		timeOutVal.tv_sec = (long)(waitTimeInMilliSec/1000); // Convert to seconds
		timeOutVal.tv_usec = (long)(waitTimeInMilliSec%1000)*1000; // Convert remainders to micro-seconds
		// socket select
		// socket read select
		retfdNum = select(0, NULL, &fdSet, NULL, &timeOutVal);
	}
	else
	{
		retfdNum = select(0, NULL, &fdSet, NULL, NULL);
	}
	if (retfdNum == SOCKET_ERROR)	// select failed
	{
		return retfdNum;
	}
	else if (retfdNum == 0)		    // select time-out
	{
		return retfdNum;
	}

	// send routine

	int sentLength=0;
	const char *packetData=packet.GetPacket();
	int length=packet.GetPacketByteSize();
	EP_ASSERT(length<=m_maxPacketSize);
	if(length>0)
	{
		int sockAddrSize=sizeof(sockaddr);
		sentLength=sendto(m_listenSocket,packetData,length,0,&clientSockAddr,sizeof(sockaddr));
	}
	return sentLength;
}

bool BaseServerUDP::SetSyncPolicy(SyncPolicy syncPolicy)
{
	if(IsServerStarted())
		return false;
	epl::LockObj lock(m_baseServerLock);
	setSyncPolicy(syncPolicy);
	return true;
}

SyncPolicy BaseServerUDP::GetSyncPolicy() const
{
	epl::LockObj lock(m_baseServerLock);
	return m_syncPolicy;
}

vector<BaseServerObject*> BaseServerUDP::GetWorkerList() const
{
	return m_workerList.GetList();
}

bool BaseServerUDP::socketCompare(sockaddr const & clientSocket, const BaseServerObject*obj )
{
	BaseServerWorkerUDP *workerObj=(BaseServerWorkerUDP*)const_cast<BaseServerObject*>(obj);
	if(clientSocket.sa_family==workerObj->m_clientSocket.sa_family)
	{
		if(System::Memcmp((void*)clientSocket.sa_data,(void*)workerObj->m_clientSocket.sa_data,sizeof(clientSocket.sa_data))==0)
			return true;
	}
	return false;
}
void BaseServerUDP::execute()
{
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

	Packet recvPacket(NULL,m_maxPacketSize);
	char *packetData=const_cast<char*>(recvPacket.GetPacket());
	int length=recvPacket.GetPacketByteSize();
	sockaddr clientSockAddr;
	int sockAddrSize=sizeof(sockaddr);
	while(m_listenSocket!=INVALID_SOCKET)
	{
		int recvLength=recvfrom(m_listenSocket,packetData,length, 0,&clientSockAddr,&sockAddrSize);

		BaseServerWorkerUDP *workerObj=(BaseServerWorkerUDP*)m_workerList.Find(clientSockAddr,socketCompare);
		if(workerObj)
		{
			if(recvLength<=0)
			{
				Packet *passPacket=EP_NEW Packet(packetData,0);
				workerObj->addPacket(passPacket);
				passPacket->ReleaseObj();
				continue;
			}	
			Packet *passPacket=EP_NEW Packet(packetData,recvLength);
			workerObj->addPacket(passPacket);
			passPacket->ReleaseObj();
		}
		else
		{
			if(recvLength<=0)
				continue;
			if(GetMaximumConnectionCount()!=CONNECTION_LIMIT_INFINITE)
			{
				if(m_workerList.Count()>=GetMaximumConnectionCount())
				{
					continue;
				}
			}
			/// Create Worker Thread
			Packet *passPacket=EP_NEW Packet(packetData,recvLength);
			BaseServerWorkerUDP *accWorker=createNewWorker();
			accWorker->setSyncPolicy(m_syncPolicy);
			if(m_syncPolicy==SYNC_POLICY_SYNCHRONOUS)
				accWorker->setParserList(m_parserList);
			BaseServerWorkerUDP::PacketPassUnit unit;
			unit.m_clientSocket=clientSockAddr;
			unit.m_server=this;
			accWorker->setPacketPassUnit(unit);
			m_workerList.Push(accWorker);
			accWorker->Start();
			accWorker->addPacket(passPacket);
			accWorker->ReleaseObj();
			passPacket->ReleaseObj();
			
		}
		
	}

	stopServer();
} 


bool BaseServerUDP::StartServer(const TCHAR * port)
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
	m_maxPacketSize=0;

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
	iHints.ai_socktype = SOCK_DGRAM;
	iHints.ai_protocol = IPPROTO_UDP;
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

	int nTmp = sizeof(int);
	getsockopt(m_listenSocket, SOL_SOCKET,SO_MAX_MSG_SIZE, (char *)&m_maxPacketSize,&nTmp);

	// Create thread 1.
	if(Start())
	{
		return true;
	}
	cleanUpServer();
	return false;


}

void BaseServerUDP::sendPacket(BaseServerObject *clientObj,unsigned int argCount,va_list args)
{
	void *argPtr=NULL;
	unsigned int waitTime=WAITTIME_INIFINITE;
	EP_ASSERT(argCount);
	argPtr = va_arg (args, void *);
	waitTime=va_arg(args,unsigned int);

	Packet *packetPtr=(Packet*)argPtr;
	((BaseServerWorkerUDP*)(clientObj))->Send(*packetPtr,waitTime);
}

void BaseServerUDP::killConnection(BaseServerObject *clientObj,unsigned int argCount,va_list args)
{
	((BaseServerWorkerUDP*)(clientObj))->KillConnection();
}

void BaseServerUDP::Broadcast(const Packet& packet, unsigned int waitTimeInMilliSec)
{
	m_workerList.Do(sendPacket,2,&packet,waitTimeInMilliSec);
}
void BaseServerUDP::CommandWorkers(void (__cdecl *DoFunc)(BaseServerObject*,unsigned int,va_list),unsigned int argCount,...)
{
	void *argPtr=NULL;
	va_list ap=NULL;
	va_start (ap , argCount);         /* Initialize the argument list. */
	m_workerList.Do(DoFunc,argCount,ap);
	va_end (ap);                  /* Clean up. */
}

void BaseServerUDP::ShutdownAllClient()
{
	epl::LockObj lock(m_baseServerLock);
	shutdownAllClient();
}

void BaseServerUDP::shutdownAllClient()
{
	m_workerList.Do(killConnection,0);
	m_workerList.Clear();
}
bool BaseServerUDP::IsServerStarted() const
{
	return (GetStatus()==Thread::THREAD_STATUS_STARTED);
}

void BaseServerUDP::StopServer()
{
	epl::LockObj lock(m_baseServerLock);
	if(!IsServerStarted())
	{
		return;
	}
	if(m_listenSocket!=INVALID_SOCKET)
	{
		int iResult;
		iResult = shutdown(m_listenSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) shutdown failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		}
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


void BaseServerUDP::cleanUpServer()
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
	m_maxPacketSize=0;
	WSACleanup();

}

void BaseServerUDP::stopServer()
{
	if(IsServerStarted())
	{
		// No longer need server socket
// 		if(m_listenSocket!=INVALID_SOCKET)
// 		{
// 			int iResult;
// 			iResult = shutdown(m_listenSocket, SD_SEND);
// 			if (iResult == SOCKET_ERROR) {
// 				epl::System::OutputDebugString(_T("%s::%s(%d)(%x) shutdown failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
// 			}
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

