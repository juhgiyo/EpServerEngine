/*! 
BaseServerUDP for the EpServerEngine
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
#include "epBaseServerUDP.h"


using namespace epse;

BaseServerUDP::BaseServerUDP(const TCHAR *  port,SyncPolicy syncPolicy,unsigned int waitTimeMilliSec, epl::LockPolicy lockPolicyType): BaseServerObject(waitTimeMilliSec,lockPolicyType)
{
	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_baseServerLock=EP_NEW epl::CriticalSectionEx();
		m_sendLock=EP_NEW epl::CriticalSectionEx();
		m_disconnectLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_baseServerLock=EP_NEW epl::Mutex();
		m_sendLock=EP_NEW epl::Mutex();
		m_disconnectLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_baseServerLock=EP_NEW epl::NoLock();
		m_sendLock=EP_NEW epl::NoLock();
		m_disconnectLock=EP_NEW epl::NoLock();
		break;
	default:
		m_baseServerLock=NULL;
		m_sendLock=NULL;
		m_disconnectLock=NULL;
		break;
	}
	SetPort(port);
	m_listenSocket=INVALID_SOCKET;
	m_result=0;
	m_maxPacketSize=0;
	setSyncPolicy(syncPolicy);
	m_parserList=NULL;

}

BaseServerUDP::BaseServerUDP(const BaseServerUDP& b):BaseServerObject(b)
{
	m_listenSocket=INVALID_SOCKET;
	m_result=0;
	m_port=b.m_port;
	m_lockPolicy=b.m_lockPolicy;
	m_maxPacketSize=b.m_maxPacketSize;
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_baseServerLock=EP_NEW epl::CriticalSectionEx();
		m_sendLock=EP_NEW epl::CriticalSectionEx();
		m_disconnectLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_baseServerLock=EP_NEW epl::Mutex();
		m_sendLock=EP_NEW epl::Mutex();
		m_disconnectLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_baseServerLock=EP_NEW epl::NoLock();
		m_sendLock=EP_NEW epl::NoLock();
		m_disconnectLock=EP_NEW epl::NoLock();
		break;
	default:
		m_baseServerLock=NULL;
		m_sendLock=NULL;
		m_disconnectLock=NULL;
		break;
	}

	m_parserList=NULL;
}
BaseServerUDP::~BaseServerUDP()
{
	StopServer();
	if(m_baseServerLock)
		EP_DELETE m_baseServerLock;
	if(m_sendLock)
		EP_DELETE m_sendLock;
	if(m_disconnectLock)
		EP_DELETE m_disconnectLock;
	if(m_parserList)
	{
		m_parserList->ReleaseObj();
	}
}

void  BaseServerUDP::SetPort(const TCHAR *  port)
{
	epl::LockObj lock(m_baseServerLock);
	setPort(port);
}

void  BaseServerUDP::setPort(const TCHAR *  port)
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



unsigned int BaseServerUDP::GetMaxPacketByteSize() const
{
	return m_maxPacketSize;
}

int BaseServerUDP::send(const Packet &packet,const sockaddr &clientSockAddr)
{

	epl::LockObj lock(m_sendLock);

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

void BaseServerUDP::execute()
{
	if(m_syncPolicy==SYNC_POLICY_SYNCHRONOUS)
	{
		m_parserList=EP_NEW ParserList(m_syncPolicy,m_waitTime,m_lockPolicy);
		if(!m_parserList || !m_parserList->StartParse())
		{
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Unable to start to Global Parser!\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
			if(m_parserList)
				m_parserList->ReleaseObj();
			m_parserList=NULL;
			stopServer(true);
			return;
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
		if(recvLength<=0)
			continue;
		/// Create Worker Thread
		Packet *passPacket=EP_NEW Packet(packetData,recvLength);
		BaseServerWorkerUDP *accWorker=createNewWorker();
		accWorker->setSyncPolicy(m_syncPolicy);
		if(m_syncPolicy==SYNC_POLICY_SYNCHRONOUS)
			accWorker->setParserList(m_parserList);
		BaseServerWorkerUDP::PacketPassUnit unit;
		unit.m_clientSocket=clientSockAddr;
		unit.m_packet=passPacket;
		unit.m_server=this;
		accWorker->setPacketPassUnit(unit);
		accWorker->Start();
		m_workerList.Push(accWorker);
		accWorker->ReleaseObj();
		passPacket->ReleaseObj();

		m_workerList.RemoveTerminated();

	}
	stopServer(true);
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

	ZeroMemory(&m_hints, sizeof(m_hints));
	m_hints.ai_family = AF_INET;
	m_hints.ai_socktype = SOCK_DGRAM;
	m_hints.ai_protocol = IPPROTO_UDP;
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

void BaseServerUDP::Broadcast(const Packet& packet)
{
	m_workerList.RemoveTerminated();
	vector<BaseServerObject*>::iterator iter;
	vector<BaseServerObject*> clientList=m_workerList.GetList();
	for(iter=clientList.begin();iter!=clientList.end();iter++)
	{
		((BaseServerWorkerUDP*)(*iter))->Send(packet);
	}
}


void BaseServerUDP::ShutdownAllClient()
{
	epl::LockObj lock(m_baseServerLock);
	shutdownAllClient();
}

void BaseServerUDP::shutdownAllClient()
{
	m_workerList.RemoveTerminated();
	vector<BaseServerObject*>::iterator iter;
	vector<BaseServerObject*> clientList=m_workerList.GetList();
	for(iter=clientList.begin();iter!=clientList.end();iter++)
	{
		((BaseServerWorkerUDP*)(*iter))->KillConnection();
	}
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
	stopServer(false);
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

void BaseServerUDP::stopServer(bool fromInternal)
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
			int iResult;
			iResult = shutdown(m_listenSocket, SD_SEND);
			if (iResult == SOCKET_ERROR) {
				epl::System::OutputDebugString(_T("%s::%s(%d)(%x) shutdown failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
			}
			closesocket(m_listenSocket);
			m_listenSocket=INVALID_SOCKET;
		}
		if(!fromInternal)
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
	}
	
	cleanUpServer();

	
	m_disconnectLock->Unlock();
}

