/*! 
IocpServerJob for the EpServerEngine

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
#include "epIocpServerJob.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

IocpServerJob::IocpServerJob(BaseSocket *socket,IocpServerJobType jobType,Packet *packet,EventEx *completionEvent,ServerCallbackInterface *callBackObj,Priority priority,epl::LockPolicy lockPolicyType):BaseJob(priority,lockPolicyType)
{
	m_jobType=jobType;
	m_packet=packet;
	if(m_packet)
		m_packet->RetainObj();
	m_socket=socket;
	if(m_socket)
		m_socket->RetainObj();

	m_completeEvent=completionEvent;
	m_callBackObj=callBackObj;
}

IocpServerJob::~IocpServerJob()
{
	if(m_packet)
		m_packet->ReleaseObj();
	if(m_socket)
		m_socket->ReleaseObj();
}
void IocpServerJob::SetJob(BaseSocket *socket,IocpServerJobType jobType, Packet *packet,EventEx *completionEvent,ServerCallbackInterface *callBackObj)
{
	if(m_socket)
		m_socket->ReleaseObj();
	m_socket=socket;
	if(m_socket)
		m_socket->RetainObj();

	m_jobType=jobType;
	if(m_packet)
		m_packet->ReleaseObj();
	m_packet=packet;
	if(m_packet)
		m_packet->RetainObj();

	m_completeEvent=completionEvent;
	m_callBackObj=callBackObj;
}

IocpServerJob::IocpServerJobType IocpServerJob::GetJobType() const
{
	return m_jobType;
}

const Packet *IocpServerJob::GetPacket() const
{
	return m_packet;
}

BaseSocket *IocpServerJob::GetSocket()
{
	return m_socket;
}

EventEx *IocpServerJob::GetCompletionEvent()
{
	return m_completeEvent;
}

ServerCallbackInterface *IocpServerJob::GetCallBackObject()
{
	return m_callBackObj;
}