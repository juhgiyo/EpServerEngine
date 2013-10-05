/*! 
BaseServerWorker for the EpServerEngine

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
#include "epBaseServerWorker.h"


#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;
BaseServerWorker::BaseServerWorker(unsigned int maximumParserCount,unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType): BaseServerSendObject(waitTimeMilliSec,lockPolicyType)
{
	m_lockPolicy=lockPolicyType;
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_sendLock=EP_NEW epl::CriticalSectionEx();
		m_baseWorkerLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_sendLock=EP_NEW epl::Mutex();
		m_baseWorkerLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_sendLock=EP_NEW epl::NoLock();
		m_baseWorkerLock=EP_NEW epl::NoLock();
		break;
	default:
		m_sendLock=NULL;
		m_baseWorkerLock=NULL;
		break;
	}
	m_recvSizePacket=Packet(NULL,4);
	m_parserList=NULL;
	m_maxParserCount=maximumParserCount;
	m_clientSocket=INVALID_SOCKET;
	m_owner=NULL;
}
BaseServerWorker::BaseServerWorker(const BaseServerWorker& b) : BaseServerSendObject(b)
{
	m_lockPolicy=b.m_lockPolicy;
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_sendLock=EP_NEW epl::CriticalSectionEx();
		m_baseWorkerLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_sendLock=EP_NEW epl::Mutex();
		m_baseWorkerLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_sendLock=EP_NEW epl::NoLock();
		m_baseWorkerLock=EP_NEW epl::NoLock();
		break;
	default:
		m_sendLock=NULL;
		m_baseWorkerLock=NULL;
		break;
	}
	m_maxParserCount=b.m_maxParserCount;
	m_clientSocket=b.m_clientSocket;
	m_recvSizePacket=b.m_recvSizePacket;
	m_owner=b.m_owner;
	m_parserList=b.m_parserList;
	if(m_parserList)
		m_parserList->RetainObj();
	
}

BaseServerWorker::~BaseServerWorker()
{
	resetWorker();
}

BaseServerWorker & BaseServerWorker::operator=(const BaseServerWorker&b)
{
	if(this!=&b)
	{
		resetWorker();

		BaseServerSendObject::operator =(b);

		m_clientSocket=INVALID_SOCKET;

		m_lockPolicy=b.m_lockPolicy;
		switch(m_lockPolicy)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_sendLock=EP_NEW epl::CriticalSectionEx();
			m_baseWorkerLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_sendLock=EP_NEW epl::Mutex();
			m_baseWorkerLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_sendLock=EP_NEW epl::NoLock();
			m_baseWorkerLock=EP_NEW epl::NoLock();
			break;
		default:
			m_sendLock=NULL;
			m_baseWorkerLock=NULL;
			break;
		}
		m_maxParserCount=b.m_maxParserCount;
		m_clientSocket=b.m_clientSocket;
		m_owner=b.m_owner;
		m_recvSizePacket=b.m_recvSizePacket;
		m_parserList=b.m_parserList;
		if(m_parserList)
			m_parserList->RetainObj();
	}
	return *this;
}

void BaseServerWorker::resetWorker()
{
	KillConnection();

	if(m_parserList)
	{
		m_parserList->ReleaseObj();
	}
	m_parserList=NULL;

	if(m_sendLock)
		EP_DELETE m_sendLock;
	m_sendLock=NULL;

	if(m_baseWorkerLock)
		EP_DELETE m_baseWorkerLock;
	m_baseWorkerLock=NULL;
	
	m_owner=NULL;
}

void BaseServerWorker::SetMaximumParserCount(unsigned int maxParserCount)
{
	epl::LockObj lock(m_baseWorkerLock);
	m_maxParserCount=maxParserCount;

}
unsigned int BaseServerWorker::GetMaximumParserCount() const
{
	epl::LockObj lock(m_baseWorkerLock);
	return m_maxParserCount;
}


void BaseServerWorker::setClientSocket(const SOCKET& clientSocket )
{
	epl::LockObj lock(m_sendLock);
	m_clientSocket=clientSocket;
}

void BaseServerWorker::setOwner(BaseServer * owner )
{
	epl::LockObj lock(m_sendLock);
	m_owner=owner;
}

int BaseServerWorker::Send(const Packet &packet, unsigned int waitTimeInMilliSec)
{	
	epl::LockObj lock(m_sendLock);
	
	if(m_clientSocket==INVALID_SOCKET)
		return 0;

	TIMEVAL	timeOutVal;
	fd_set	fdSet;
	int		retfdNum = 0;

	FD_ZERO(&fdSet);
	FD_SET(m_clientSocket, &fdSet);
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
	else if (retfdNum == 0)		// select time-out
	{
		return retfdNum;
	}

	// send routine
	int writeLength=0;
	const char *packetData=packet.GetPacket();
	int length=packet.GetPacketByteSize();

	if(length>0)
	{
		int sentLength=send(m_clientSocket,reinterpret_cast<char*>(&length),4,0);
		if(sentLength<=0)
			return sentLength;
	}
	while(length>0)
	{
		int sentLength=send(m_clientSocket,packetData,length,0);
		writeLength+=sentLength;
		if(writeLength<=0)
			break;
		length-=sentLength;
		packetData+=sentLength;
	}
	return writeLength;
}
vector<BaseServerObject*> BaseServerWorker::GetPacketParserList() const
{
	if(m_parserList)
		return m_parserList->GetList();
	return vector<BaseServerObject*>();
}
bool BaseServerWorker::IsConnectionAlive() const
{
	return (GetStatus()==Thread::THREAD_STATUS_STARTED);
}

BaseServer *BaseServerWorker::GetOwner() const
{
	return m_owner;
}

epl::EpTString BaseServerWorker::GetIP() const
{
	sockaddr socketAddr;
	int socketAddrSize=sizeof(sockaddr);
	TCHAR ip[INET6_ADDRSTRLEN] = {0};
	unsigned long ipSize=INET6_ADDRSTRLEN;
	if(getpeername(m_clientSocket,&socketAddr,&socketAddrSize)!=SOCKET_ERROR)
	{
		WSAPROTOCOL_INFO protocolInfo;
		WSAAddressToString(&socketAddr,sizeof(sockaddr),&protocolInfo,ip,&ipSize);
	}
	epl::EpTString retString=ip;
	return retString;
}

sockaddr BaseServerWorker::GetSockAddr() const
{
	sockaddr socketAddr={0};
	int socketAddrSize=sizeof(sockaddr);
	if(getpeername(m_clientSocket,&socketAddr,&socketAddrSize)!=SOCKET_ERROR)
	{
		return socketAddr;
	}
	return socketAddr;
}

void BaseServerWorker::KillConnection()
{
	epl::LockObj lock(m_baseWorkerLock);
	if(!IsConnectionAlive())
	{
		return;
	}
	// No longer need client socket
	m_sendLock->Lock();
	if(m_clientSocket!=INVALID_SOCKET)
	{
		int iResult;
		iResult = shutdown(m_clientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) shutdown failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		}
		closesocket(m_clientSocket);
		m_clientSocket = INVALID_SOCKET;
	}
	m_sendLock->Unlock();

	TerminateAfter(m_waitTime);

	if(m_parserList)
	{
		if(m_syncPolicy==SYNC_POLICY_SYNCHRONOUS_BY_CLIENT)
		{
			m_parserList->StopParse();
		}

		if(m_syncPolicy!=SYNC_POLICY_SYNCHRONOUS)
		{
			m_parserList->Clear();
		}

		m_parserList->ReleaseObj();
		m_parserList=NULL;
	}	

	removeSelfFromContainer();
}


void BaseServerWorker::killConnection()
{
	if(IsConnectionAlive())
	{
		// No longer need client socket
		m_sendLock->Lock();
		if(m_clientSocket!=INVALID_SOCKET)
		{
			closesocket(m_clientSocket);
			m_clientSocket = INVALID_SOCKET;
		}
		m_sendLock->Unlock();
	
		if(m_parserList)
		{
			if(m_syncPolicy==SYNC_POLICY_SYNCHRONOUS_BY_CLIENT)
			{
				m_parserList->StopParse();
			}

			if(m_syncPolicy!=SYNC_POLICY_SYNCHRONOUS)
			{
				m_parserList->Clear();
			}

			m_parserList->ReleaseObj();
			m_parserList=NULL;
		}	

		removeSelfFromContainer();
	
	}
}


int BaseServerWorker::receive(Packet &packet)
{
	int readLength=0;
	int length=packet.GetPacketByteSize();
	char *packetData=const_cast<char*>(packet.GetPacket());
	while(length>0)
	{
		int recvLength=recv(m_clientSocket,packetData, length, 0);
		readLength+=recvLength;
		if(recvLength<=0)
			break;
		length-=recvLength;
		packetData+=recvLength;
	}
	return readLength;
}

void BaseServerWorker::setParserList(ParserList *parserList)
{
	if(!parserList)
		return;
	if(m_parserList)
		m_parserList->ReleaseObj();
	m_parserList=parserList;
	if(m_parserList)
		m_parserList->RetainObj();

}

void BaseServerWorker::execute()
{
	int iResult=0;

	if(!m_parserList)
		m_parserList=EP_NEW ParserList(m_syncPolicy,m_waitTime,m_lockPolicy);

	if(m_parserList&&m_syncPolicy==SYNC_POLICY_SYNCHRONOUS_BY_CLIENT)
	{
		m_parserList->StartParse();
	}


	// Receive until the peer shuts down the connection
	do {
		iResult =receive(m_recvSizePacket);
		if(iResult>0)
		{
			unsigned int shouldReceive=(reinterpret_cast<unsigned int*>(const_cast<char*>(m_recvSizePacket.GetPacket())))[0];
			Packet *recvPacket=EP_NEW Packet(NULL,shouldReceive);
			iResult = receive(*recvPacket);

			if (iResult == shouldReceive) {
				BasePacketParser::PacketPassUnit passUnit;
				passUnit.m_packet=recvPacket;
				passUnit.m_owner=this;
				BasePacketParser *parser =createNewPacketParser();
				if(!parser)
				{
					recvPacket->ReleaseObj();
					continue;
				}
				parser->setSyncPolicy(m_syncPolicy);
				parser->setPacketPassUnit(passUnit);
				m_parserList->Push(parser);
				if(m_syncPolicy==SYNC_POLICY_ASYNCHRONOUS)
					parser->Start();
				parser->ReleaseObj();
				recvPacket->ReleaseObj();
				if(GetMaximumParserCount()!=PARSER_LIMIT_INFINITE)
				{
					while(m_parserList->Count()>=GetMaximumParserCount())
					{
						m_parserList->WaitForListSizeDecrease();
					}
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

	killConnection();
}


