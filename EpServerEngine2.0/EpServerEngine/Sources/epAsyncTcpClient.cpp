/*! 
AsyncTcpClient for the EpServerEngine
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
#include "epAsyncTcpClient.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

AsyncTcpClient::AsyncTcpClient(ClientCallbackInterface *callBackObj,const TCHAR * hostName, const TCHAR * port,bool isAsynchronousReceive,unsigned int waitTimeMilliSec,unsigned int maximumProcessorCount,epl::LockPolicy lockPolicyType) :BaseServerObject(waitTimeMilliSec,lockPolicyType)
{
	EP_ASSERT(callBackObj);

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
	m_recvSizePacket=Packet(NULL,4);
	SetHostName(hostName);
	SetPort(port);
	m_connectSocket=INVALID_SOCKET;
	m_result=0;

	m_processorList=ServerObjectList(waitTimeMilliSec,lockPolicyType);
	m_maxProcessorCount=maximumProcessorCount;
	m_callBackObj=callBackObj;
	m_isAsynchronousReceive=isAsynchronousReceive;
}

AsyncTcpClient::AsyncTcpClient(const AsyncTcpClient& b) :BaseServerObject(b)
{
	m_connectSocket=INVALID_SOCKET;
	m_result=0;

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
	m_recvSizePacket=b.m_recvSizePacket;
	
	m_processorList=b.m_processorList;
	m_maxProcessorCount=b.m_maxProcessorCount;
	m_callBackObj=b.m_callBackObj;
	m_isAsynchronousReceive=b.m_isAsynchronousReceive;
	
}
AsyncTcpClient::~AsyncTcpClient()
{
	resetClient();
}

AsyncTcpClient & AsyncTcpClient::operator=(const AsyncTcpClient&b)
{
	if(this!=&b)
	{
		resetClient();

		BaseServerObject::operator =(b);

		m_connectSocket=INVALID_SOCKET;
		m_result=0;

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
		m_recvSizePacket=b.m_recvSizePacket;

		m_processorList=b.m_processorList;
		m_maxProcessorCount=b.m_maxProcessorCount;
		m_callBackObj=b.m_callBackObj;
		m_isAsynchronousReceive=b.m_isAsynchronousReceive;
	}
	return *this;
}

void AsyncTcpClient::resetClient()
{
	Disconnect();

	if(m_sendLock)
		EP_DELETE m_sendLock;
	if(m_generalLock)
		EP_DELETE m_generalLock;
	m_sendLock=NULL;
	m_generalLock=NULL;
}

void  AsyncTcpClient::SetHostName(const TCHAR * hostName)
{
	epl::LockObj lock(m_generalLock);
	setHostName(hostName);

}

void  AsyncTcpClient::SetPort(const TCHAR *port)
{
	epl::LockObj lock(m_generalLock);
	setPort(port);
}

void AsyncTcpClient::setHostName(const TCHAR * hostName)
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

void AsyncTcpClient::setPort(const TCHAR *port)
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

void AsyncTcpClient::SetMaximumProcessorCount(unsigned int maxProcessorCount)
{
	epl::LockObj lock(m_generalLock);
	m_maxProcessorCount=maxProcessorCount;

}
unsigned int AsyncTcpClient::GetMaximumProcessorCount() const
{
	epl::LockObj lock(m_generalLock);
	return m_maxProcessorCount;
}

epl::EpTString AsyncTcpClient::GetHostName() const
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
epl::EpTString AsyncTcpClient::GetPort() const
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

void AsyncTcpClient::SetWaitTime(unsigned int milliSec)
{
	m_waitTime=milliSec;
	m_processorList.SetWaitTime(milliSec);
}

int AsyncTcpClient::Send(const Packet &packet, unsigned int waitTimeInMilliSec)
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
	int writeLength=0;
	const char *packetData=packet.GetPacket();
	int length=packet.GetPacketByteSize();

	if(length>0)
	{
		int sentLength=send(m_connectSocket,reinterpret_cast<char*>(&length),4,0);
		if(sentLength<=0)
			return sentLength;
	}
	while(length>0)
	{
		int sentLength=send(m_connectSocket,packetData,length,0);
		writeLength+=sentLength;
		if(sentLength<=0)
		{
			return writeLength;
		}
		length-=sentLength;
		packetData+=sentLength;
	}
	return writeLength;
}

void AsyncTcpClient::SetCallbackObject(ClientCallbackInterface *callBackObj)
{
	EP_ASSERT(callBackObj);
	m_callBackObj=callBackObj;
}
ClientCallbackInterface *AsyncTcpClient::GetCallbackObject()
{
	return m_callBackObj;
}
bool AsyncTcpClient::GetIsAsynchronousReceive() const
{
	return m_isAsynchronousReceive;
}
void AsyncTcpClient::SetIsAsynchronousReceive(bool isASynchronousReceive)
{
	m_isAsynchronousReceive=isASynchronousReceive;
}

void AsyncTcpClient::execute() 
{
	int iResult;
	// Receive until the peer shuts down the connection
	do {
		int size =receive(m_recvSizePacket);
		if(size>0)
		{
			unsigned int shouldReceive=(reinterpret_cast<unsigned int*>(const_cast<char*>(m_recvSizePacket.GetPacket())))[0];
			Packet *recvPacket=EP_NEW Packet(NULL,shouldReceive);
			iResult = receive(*recvPacket);

			if (iResult == shouldReceive) {
				if(m_isAsynchronousReceive)
				{
					ClientPacketProcessor::PacketPassUnit passUnit;
					passUnit.m_packet=recvPacket;
					passUnit.m_owner=this;
					ClientPacketProcessor *parser =EP_NEW ClientPacketProcessor(m_callBackObj,m_waitTime,m_lockPolicy);
					EP_ASSERT(parser);
					parser->setPacketPassUnit(passUnit);
					m_processorList.Push(parser);
					parser->ReleaseObj();
					recvPacket->ReleaseObj();
					if(GetMaximumProcessorCount()!=PROCESSOR_LIMIT_INFINITE)
					{
						while(m_processorList.Count()>=GetMaximumProcessorCount())
						{
							m_processorList.WaitForListSizeDecrease();
						}
					}
				}
				else
				{
					m_callBackObj->OnReceived(reinterpret_cast<ClientInterface*>(this),*recvPacket);
					recvPacket->ReleaseObj();
				}
				
			}
			else if (iResult == 0)
			{
				epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Connection closing...\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
				recvPacket->ReleaseObj();
				break;
			}
			else  {
				epl::System::OutputDebugString(_T("%s::%s(%d)(%x) recv failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
				recvPacket->ReleaseObj();
				break;
			}
		}
		else
		{
			break;
		}

	} while (iResult > 0);

	disconnect();
}


int AsyncTcpClient::receive(Packet &packet)
{

	int readLength=0;
	int length=packet.GetPacketByteSize();
	char *packetData=const_cast<char*>(packet.GetPacket());
	while(length>0)
	{
		int recvLength=recv(m_connectSocket,packetData, length, 0);
		readLength+=recvLength;
		if(recvLength<=0)
			break;
		length-=recvLength;
		packetData+=recvLength;
	}
	return readLength;
}

bool AsyncTcpClient::Connect(const TCHAR * hostName, const TCHAR * port)
{
	LockObj lock(m_generalLock);
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
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(m_hostName.c_str(), m_port.c_str(), &hints, &m_result);
	if ( iResult != 0 ) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) getaddrinfo failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		WSACleanup();
		return false;
	}

	struct addrinfo *iPtr=0;
	// Attempt to connect to an address until one succeeds
	for(iPtr=m_result; iPtr != NULL ;iPtr=iPtr->ai_next) {

		// Create a SOCKET for connecting to server
		m_connectSocket = socket(iPtr->ai_family, iPtr->ai_socktype, 
			iPtr->ai_protocol);
		if (m_connectSocket == INVALID_SOCKET) {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Socket failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
			cleanUpClient();
			return false;
		}

		// Connect to server.
		iResult = connect( m_connectSocket, iPtr->ai_addr, static_cast<int>(iPtr->ai_addrlen));
		if (iResult == SOCKET_ERROR) {
			closesocket(m_connectSocket);
			m_connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	if (m_connectSocket == INVALID_SOCKET) {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Unable to connect to server!\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		cleanUpClient();
		return false;
	}
	if(Start())
	{
		return true;
	}
	cleanUpClient();
	return false;
}


bool AsyncTcpClient::IsConnected() const
{
	return (GetStatus()==Thread::THREAD_STATUS_STARTED);
}

void AsyncTcpClient::cleanUpClient()
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
	WSACleanup();

}

void AsyncTcpClient::disconnect()
{
	if(IsConnected())
	{
		m_sendLock->Lock();
		if(m_connectSocket!=INVALID_SOCKET)
		{
 			closesocket(m_connectSocket);
 			m_connectSocket = INVALID_SOCKET;

		}
		m_sendLock->Unlock();

		m_processorList.Clear();
	}

	cleanUpClient();
	m_callBackObj->OnDisconnect(reinterpret_cast<ClientInterface*>(this));
}

void AsyncTcpClient::Disconnect()
{
	epl::LockObj lock(m_generalLock);
	if(!IsConnected())
	{
		return;
	}
	if(m_connectSocket!=INVALID_SOCKET)
	{
		// shutdown the connection since no more data will be sent
		int iResult = shutdown(m_connectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) shutdown failed with error: %d\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this, WSAGetLastError());
		}
		closesocket(m_connectSocket);
		m_connectSocket = INVALID_SOCKET;

	}
	TerminateAfter(m_waitTime);

	m_processorList.Clear();
	cleanUpClient();
	m_callBackObj->OnDisconnect(reinterpret_cast<ClientInterface*>(this));

}



