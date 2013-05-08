/*! 
IocpUdpClient for the EpServerEngine
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
#include "epIocpUdpClient.h"
#include "epIocpClientProcessor.h"
#include "epIocpClientJob.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)


using namespace epse;

IocpUdpClient::IocpUdpClient(epl::LockPolicy lockPolicyType): BaseUdpClient(lockPolicyType)
{
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_workerLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_workerLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_workerLock=EP_NEW epl::NoLock();
		break;
	default:
		m_workerLock=NULL;
		break;
	}

	m_isConnected=false;

}


IocpUdpClient::IocpUdpClient(const IocpUdpClient& b):BaseUdpClient(b)
{
	switch(m_lockPolicy)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_workerLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_workerLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_workerLock=EP_NEW epl::NoLock();
		break;
	default:
		m_workerLock=NULL;
		break;
	}

	m_isConnected=false;
}
IocpUdpClient::~IocpUdpClient()
{
	if(m_workerLock)
		EP_DELETE m_workerLock;
}
IocpUdpClient & IocpUdpClient::operator=(const IocpUdpClient&b)
{
	if(this!=&b)
	{				

		BaseUdpClient::operator =(b);

		switch(m_lockPolicy)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_workerLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_workerLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_workerLock=EP_NEW epl::NoLock();
			break;
		default:
			m_workerLock=NULL;
			break;
		}

		m_isConnected=false;
	}
	return *this;
}




bool IocpUdpClient::Connect(const ClientOps &ops)
{
	m_workerLock->Lock();
	while(!m_emptyWorkerList.empty())
		m_emptyWorkerList.pop();

	for(int trav=0;trav<m_workerList.size();trav++)
	{
		m_workerList.at(trav)->TerminateWorker(m_waitTime);
		EP_DELETE m_workerList.at(trav);
	}
	m_workerList.clear();

	int workerCount=ops.workerThreadCount;
	if(workerCount==0)
	{
		workerCount=System::GetNumberOfCores()*2;
	}
	for(int trav=0;trav<workerCount;trav++)
	{
		BaseWorkerThread *workerThread=WorkerThreadFactory::GetWorkerThread(BaseWorkerThread::THREAD_LIFE_SUSPEND_AFTER_WORK);

		workerThread->SetCallBackClass(this);

		m_workerList.push_back(workerThread);
		m_emptyWorkerList.push(workerThread);
		workerThread->SetJobProcessor(EP_NEW IocpClientProcessor());
		workerThread->Start();
	}
	m_workerLock->Unlock();

	epl::LockObj lock(m_generalLock);
	if(IsConnectionAlive())
		return true;

	if(ops.callBackObj)
		m_callBackObj=ops.callBackObj;
	EP_ASSERT(m_callBackObj);

	if(ops.hostName)
	{
		setHostName(ops.hostName);
	}
	if(ops.port)
	{
		setPort(ops.port);
	}

	if(!m_port.length())
	{
		m_port=DEFAULT_PORT;
	}

	if(!m_hostName.length())
	{
		m_hostName=DEFAULT_HOSTNAME;
	}
	SetWaitTime(ops.waitTimeMilliSec);

	WSADATA wsaData;
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
	m_isConnected=true;
	return true;
}


bool IocpUdpClient::IsConnectionAlive() const
{
	return m_isConnected;
}

void IocpUdpClient::Disconnect()
{
	epl::LockObj lock(m_generalLock);
	if(!IsConnectionAlive())
	{
		return;
	}
	m_isConnected=false;
	if(m_connectSocket!=INVALID_SOCKET)
	{
		// shutdown the connection since no more data will be sent
		int iResult = shutdown(m_connectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR){
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) shutdown failed with error: %d\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this, WSAGetLastError());
		}
	}
	else
	{
		return;
	}
	cleanUpClient();
	
	m_workerLock->Lock();
	while(!m_emptyWorkerList.empty())
		m_emptyWorkerList.pop();

	for(int trav=0;trav<m_workerList.size();trav++)
	{
		m_workerList.at(trav)->TerminateWorker(m_waitTime);
		EP_DELETE m_workerList.at(trav);
	}
	m_workerList.clear();
	m_workerLock->Unlock();


	m_callBackObj->OnDisconnect(this);
}

void IocpUdpClient::execute()
{}


void IocpUdpClient::disconnect()
{
	if(IsConnectionAlive())
	{
		// No longer need client socket
		m_isConnected=false;
		cleanUpClient();
	
		m_workerLock->Lock();
		while(!m_emptyWorkerList.empty())
			m_emptyWorkerList.pop();

		for(int trav=0;trav<m_workerList.size();trav++)
		{
			m_workerList.at(trav)->TerminateWorker(m_waitTime);
			EP_DELETE m_workerList.at(trav);
		}
		m_workerList.clear();
		m_workerLock->Unlock();

		m_callBackObj->OnDisconnect(this);		
	}

}

void IocpUdpClient::Send(Packet &packet,EventEx *completionEvent,ClientCallbackInterface *callBackObj,Priority priority)
{
	IocpClientJob *newJob= EP_NEW IocpClientJob(this,IocpClientJob::IOCP_CLIENT_JOB_TYPE_SEND,&packet,completionEvent,callBackObj,priority,m_lockPolicy);
	pushJob(newJob);
	newJob->ReleaseObj();
}

void IocpUdpClient::Receive(EventEx *completionEvent,ClientCallbackInterface *callBackObj,Priority priority)
{
	IocpClientJob *newJob= EP_NEW IocpClientJob(this,IocpClientJob::IOCP_CLIENT_JOB_TYPE_RECEIVE,NULL,completionEvent,callBackObj,priority,m_lockPolicy);
	pushJob(newJob);
	newJob->ReleaseObj();
}

int IocpUdpClient::Send(const Packet &packet, unsigned int waitTimeInMilliSec,SendStatus *sendStatus)
{
	epl::LockObj lock(m_generalLock);
	return BaseUdpClient::Send(packet,waitTimeInMilliSec,sendStatus);
}


Packet *IocpUdpClient::Receive(unsigned int waitTimeInMilliSec,ReceiveStatus *retStatus)
{
	epl::LockObj lock(m_generalLock);
	if(!IsConnectionAlive())
	{
		if(retStatus)
			*retStatus=RECEIVE_STATUS_FAIL_NOT_CONNECTED;
		return NULL;
	}

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
		retfdNum = select(0,&fdSet, NULL, NULL, &timeOutVal);
	}
	else
	{
		retfdNum = select(0, &fdSet,NULL, NULL, NULL);
	}
	if (retfdNum == SOCKET_ERROR)	// select failed
	{
		disconnect();
		if(retStatus)
			*retStatus=RECEIVE_STATUS_FAIL_SOCKET_ERROR;
		return NULL;
	}
	else if (retfdNum == 0)		    // select time-out
	{
		if(retStatus)
			*retStatus=RECEIVE_STATUS_FAIL_TIME_OUT;
		return NULL;
	}

	int iResult=0;
	Packet recvPacket(NULL,m_maxPacketSize);
	iResult = receive(recvPacket);

	if (iResult > 0) {
		Packet *passPacket=EP_NEW Packet(recvPacket.GetPacket(),iResult);
		if(retStatus)
			*retStatus=RECEIVE_STATUS_SUCCESS;
		return passPacket;
	}
	else if (iResult == 0)
	{
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Connection closing...\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		disconnect();
		if(retStatus)
			*retStatus=RECEIVE_STATUS_FAIL_CONNECTION_CLOSING;
		return NULL;
	}
	else  {
		epl::System::OutputDebugString(_T("%s::%s(%d)(%x) recv failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		disconnect();
		if(retStatus)
			*retStatus=RECEIVE_STATUS_FAIL_RECEIVE_FAILED;
		return NULL;
	}
}


void IocpUdpClient::CallBackFunc(BaseWorkerThread *p)
{
	epl::LockObj lock(m_workerLock);
	m_emptyWorkerList.push(p);
}

void IocpUdpClient::pushJob(BaseJob * job)
{
	epl::LockObj lock(m_workerLock);
	if(m_emptyWorkerList.size())
	{
		m_emptyWorkerList.front()->Push(job);
		m_emptyWorkerList.pop();
	}
	else
	{
		if(!m_workerList.size())
		{
			return;
		}

		size_t jobCount=m_workerList.at(0)->GetJobCount();
		int workerIdx=0;	

		for(int trav=1;trav<m_workerList.size();trav++)
		{
			if(m_workerList.at(trav)->GetJobCount()<jobCount)
			{
				jobCount=m_workerList.at(trav)->GetJobCount();
				workerIdx=trav;
			}
		}
		m_workerList.at(workerIdx)->Push(job);
	}
}
