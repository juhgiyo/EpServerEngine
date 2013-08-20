/*! 
BaseClientUDP for the EpServerEngine

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
#include "epBaseClientUDP.h"


#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

BaseClientUDP::BaseClientUDP(const TCHAR * hostName, const TCHAR * port,SyncPolicy syncPolicy,unsigned int maximumParserCount,unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType): BaseServerSendObject(waitTimeMilliSec,lockPolicyType)
{
	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_sendLock=EP_NEW epl::CriticalSectionEx();
		m_generalLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_sendLock=EP_NEW epl::Mutex();
		m_generalLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_sendLock=EP_NEW epl::NoLock();
		m_generalLock=EP_NEW epl::NoLock();
		break;
	default:
		m_sendLock=NULL;
		m_generalLock=NULL;
		break;
	}
	SetHostName(hostName);
	SetPort(port);
	m_maxParserCount=maximumParserCount;
	m_connectSocket=INVALID_SOCKET;
	m_result=0;
	m_ptr=0;
	m_maxPacketSize=0;
	setSyncPolicy(syncPolicy);
	m_parserList=ParserList(syncPolicy,waitTimeMilliSec,lockPolicyType);
}

BaseClientUDP::BaseClientUDP(const BaseClientUDP& b):BaseServerSendObject(b)
{
	m_connectSocket=INVALID_SOCKET;
	m_result=0;
	m_ptr=0;

	m_lockPolicy=b.m_lockPolicy;
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_sendLock=EP_NEW epl::CriticalSectionEx();
		m_generalLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_sendLock=EP_NEW epl::Mutex();
		m_generalLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_sendLock=EP_NEW epl::NoLock();
		m_generalLock=EP_NEW epl::NoLock();
		break;
	default:
		m_sendLock=NULL;
		m_generalLock=NULL;
		break;
	}
	
	LockObj lock(b.m_generalLock);
	m_hostName=b.m_hostName;
	m_port=b.m_port;
	m_maxParserCount=b.m_maxParserCount;
	m_maxPacketSize=b.m_maxPacketSize;
	m_parserList=b.m_parserList;

}
BaseClientUDP::~BaseClientUDP()
{
	resetClient();
}

BaseClientUDP & BaseClientUDP::operator=(const BaseClientUDP&b)
{
	if(this!=&b)
	{				
	
		resetClient();

		BaseServerSendObject::operator =(b);

		m_connectSocket=INVALID_SOCKET;
		m_result=0;
		m_ptr=0;

		m_lockPolicy=b.m_lockPolicy;
		switch(m_lockPolicy)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_sendLock=EP_NEW epl::CriticalSectionEx();
			m_generalLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_sendLock=EP_NEW epl::Mutex();
			m_generalLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_sendLock=EP_NEW epl::NoLock();
			m_generalLock=EP_NEW epl::NoLock();
			break;
		default:
			m_sendLock=NULL;
			m_generalLock=NULL;
			break;
		}

		LockObj lock(b.m_generalLock);
		m_hostName=b.m_hostName;
		m_port=b.m_port;
		m_maxParserCount=b.m_maxParserCount;
		m_maxPacketSize=b.m_maxPacketSize;
		m_parserList=b.m_parserList;


	}
	return *this;
}

void BaseClientUDP::resetClient()
{
	Disconnect();

	if(m_sendLock)
		EP_DELETE m_sendLock;
	m_sendLock=NULL;
	if(m_generalLock)
		EP_DELETE m_generalLock;
	m_generalLock=NULL;
	
}
void  BaseClientUDP::SetHostName(const TCHAR * hostName)
{
	epl::LockObj lock(m_generalLock);
	setHostName(hostName);

}

void  BaseClientUDP::SetPort(const TCHAR *port)
{
	epl::LockObj lock(m_generalLock);
	setPort(port);
}

void BaseClientUDP::setHostName(const TCHAR * hostName)
{
	if(hostName==NULL)
	{
		m_hostName=DEFAULT_HOSTNAME;
		return;
	}
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

void BaseClientUDP::setPort(const TCHAR *port)
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
epl::EpTString BaseClientUDP::GetHostName() const
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
epl::EpTString BaseClientUDP::GetPort() const
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

void BaseClientUDP::SetMaximumParserCount(unsigned int maxParserCount)
{
	epl::LockObj lock(m_generalLock);
	m_maxParserCount=maxParserCount;

}
unsigned int BaseClientUDP::GetMaximumParserCount() const
{
	epl::LockObj lock(m_generalLock);
	return m_maxParserCount;
}

bool BaseClientUDP::SetSyncPolicy(SyncPolicy syncPolicy)
{
	if(IsConnected())
		return false;
	epl::LockObj lock(m_generalLock);
	setSyncPolicy(syncPolicy);
	m_parserList=ParserList(syncPolicy,m_waitTime,m_lockPolicy);
	return true;
}

SyncPolicy BaseClientUDP::GetSyncPolicy() const
{
	epl::LockObj lock(m_generalLock);
	return m_syncPolicy;
}

void BaseClientUDP::SetWaitTime(unsigned int milliSec)
{
	m_waitTime=milliSec;
	m_parserList.SetWaitTime(milliSec);
}

unsigned int BaseClientUDP::GetMaxPacketByteSize() const
{
	return m_maxPacketSize;
}

int BaseClientUDP::Send(const Packet &packet, unsigned int waitTimeInMilliSec)
{
	epl::LockObj lock(m_sendLock);
	if(!IsConnected())
		return 0;

	// select routine
	TIMEVAL	timeOutVal;
	fd_set	fdSet;
	int		retfdNum = 0;

	FD_ZERO(&fdSet);
	FD_SET(m_connectSocket, &fdSet);
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
		//int sentLength=send(m_connectSocket,packetData,length,0);
		sentLength=sendto(m_connectSocket,packetData,length,0,m_ptr->ai_addr,sizeof(sockaddr));
		if(sentLength<=0)
		{
			return sentLength;
		}
	}
	return sentLength;
}

int BaseClientUDP::receive(Packet &packet)
{

	int length=packet.GetPacketByteSize();
	char *packetData=const_cast<char*>(packet.GetPacket());
	sockaddr tmpInfo;
	int tmpInfoSize=sizeof(sockaddr);
	int recvLength = recvfrom(m_connectSocket,packetData,length,0,&tmpInfo,&tmpInfoSize);
	return recvLength;
}


vector<BaseServerObject*> BaseClientUDP::GetPacketParserList() const
{
	return m_parserList.GetList();
}


bool BaseClientUDP::Connect(const TCHAR * hostName, const TCHAR * port)
{
	epl::LockObj lock(m_generalLock);
	if(IsConnected())
		return true;

	if(hostName)
	{
		setHostName(hostName);
	}
	if(port)
	{
		setPort(port);
	}

	if(!m_port.length())
	{
		m_port=DEFAULT_PORT;
	}

	if(!m_hostName.length())
	{
		m_hostName=DEFAULT_HOSTNAME;
	}


	WSADATA wsaData;
	m_connectSocket = INVALID_SOCKET;
	m_maxPacketSize=0;
	struct addrinfo hints;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) WSAStartup failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		return false;
	}

	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	// Resolve the server address and port
	iResult = getaddrinfo(m_hostName.c_str(), m_port.c_str(), &hints, &m_result);
	if ( iResult != 0 ) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) getaddrinfo failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		WSACleanup();
		return false;
	}

	// Attempt to connect to an address until one succeeds
	for(m_ptr=m_result; m_ptr != NULL ;m_ptr=m_ptr->ai_next) {

		// Create a SOCKET for connecting to server
		m_connectSocket = socket(m_ptr->ai_family, m_ptr->ai_socktype, 
			m_ptr->ai_protocol);
		if (m_connectSocket == INVALID_SOCKET) {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Socket failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
			cleanUpClient();
			return false;
		}
		break;
	}
	if (m_connectSocket == INVALID_SOCKET) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Unable to connect to server!\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		cleanUpClient();
		return false;
	}

	int nTmp = sizeof(int);
	getsockopt(m_connectSocket, SOL_SOCKET,SO_MAX_MSG_SIZE, (char *)&m_maxPacketSize,&nTmp);

	if(Start())
	{
		if(m_syncPolicy==SYNC_POLICY_SYNCHRONOUS)
		{
			if(!m_parserList.StartParse())
			{
				epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Unable to start to Global Parser!\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
				disconnect();
				return false;
			}
		}
		return true;
	}
	cleanUpClient();
	return false;
}


bool BaseClientUDP::IsConnected() const
{
	return (GetStatus()==Thread::THREAD_STATUS_STARTED);
}

void BaseClientUDP::cleanUpClient()
{
	if(m_connectSocket!=INVALID_SOCKET)
	{
		closesocket(m_connectSocket);
		m_connectSocket = INVALID_SOCKET;
	}

	if(m_result)
	{
		freeaddrinfo(m_result);
		m_result=NULL;
	}

	m_maxPacketSize=0;
	WSACleanup();

}

void BaseClientUDP::disconnect()
{
	if(IsConnected())
	{
// 		if(m_connectSocket!=INVALID_SOCKET)
// 		{
// 			int iResult = shutdown(m_connectSocket, SD_SEND);
// 			if (iResult == SOCKET_ERROR)
// 				epl::System::OutputDebugString(_T("%s::%s(%d)(%x) shutdown failed with error: %d\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this, WSAGetLastError());
// 	 		closesocket(m_connectSocket);
//  			m_connectSocket = INVALID_SOCKET;
// 		}
		
		if(m_syncPolicy==SYNC_POLICY_SYNCHRONOUS)
		{
			m_parserList.StopParse();
		}
		m_parserList.Clear();
	}
	cleanUpClient();
	
}

void BaseClientUDP::Disconnect()
{
	epl::LockObj lock(m_generalLock);
	if(!IsConnected())
	{
		return;
	}
	if(m_connectSocket!=INVALID_SOCKET)
	{
		int iResult = shutdown(m_connectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR)
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) shutdown failed with error: %d\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this, WSAGetLastError());
		closesocket(m_connectSocket);
		m_connectSocket = INVALID_SOCKET;
	}

	TerminateAfter(m_waitTime);

	if(m_syncPolicy==SYNC_POLICY_SYNCHRONOUS)
	{
		m_parserList.StopParse();
	}
	m_parserList.Clear();
	cleanUpClient();

}


void BaseClientUDP::execute() 
{
	int iResult=0;
	// Receive until the peer shuts down the connection
	Packet recvPacket(NULL,m_maxPacketSize);
	do {
		iResult = receive(recvPacket);
		
		if (iResult > 0) {
			BasePacketParser::PacketPassUnit passUnit;
			Packet *passPacket=EP_NEW Packet(recvPacket.GetPacket(),iResult);
			passUnit.m_packet=passPacket;
			passUnit.m_owner=this;
			BasePacketParser *parser=createNewPacketParser();
			parser->setSyncPolicy(m_syncPolicy);
			parser->setPacketPassUnit(passUnit);
			if(m_syncPolicy==SYNC_POLICY_ASYNCHRONOUS)
				parser->Start();
			m_parserList.Push(parser);
			parser->ReleaseObj();
			passPacket->ReleaseObj();
			if(GetMaximumParserCount()!=PARSER_LIMIT_INFINITE)
			{
				while(m_parserList.Count()>=GetMaximumParserCount())
				{
					m_parserList.WaitForListSizeDecrease();
				}
			}
		}
		else if (iResult == 0)
		{
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Connection closing...\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
			break;
		}
		else  {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) recv failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
			break;
		}

	} while (iResult > 0);
	
	disconnect();
}