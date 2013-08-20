/*! 
IocpClientJob for the EpServerEngine

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
#include "epIocpClientJob.h"
#include "epBaseClient.h"
#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

IocpClientJob::IocpClientJob(BaseClient *client,IocpClientJobType jobType,Packet *packet,EventEx *completionEvent,ClientCallbackInterface *callBackObj,Priority priority,epl::LockPolicy lockPolicyType):BaseJob(priority,lockPolicyType)
{
	m_client=client;
	if(m_client)
		m_client->RetainObj();
	m_jobType=jobType;
	m_packet=packet;
	if(m_packet)
		m_packet->RetainObj();

	m_completeEvent=completionEvent;
	m_callBackObj=callBackObj;
}

IocpClientJob::~IocpClientJob()
{
	if(m_client)
		m_client->ReleaseObj();
	if(m_packet)
		m_packet->ReleaseObj();
}
void IocpClientJob::SetJob(BaseClient *client,IocpClientJobType jobType, Packet *packet,EventEx *completionEvent,ClientCallbackInterface *callBackObj)
{
	if(m_client)
		m_client->ReleaseObj();
	m_client=client;
	if(m_client)
		m_client->RetainObj();


	m_jobType=jobType;
	if(m_packet)
		m_packet->ReleaseObj();
	m_packet=packet;
	if(m_packet)
		m_packet->RetainObj();

	m_completeEvent=completionEvent;
	m_callBackObj=callBackObj;
}

IocpClientJob::IocpClientJobType IocpClientJob::GetJobType() const
{
	return m_jobType;
}

const Packet *IocpClientJob::GetPacket() const
{
	return m_packet;
}

BaseClient *IocpClientJob::GetClient()
{
	return m_client;
}
EventEx *IocpClientJob::GetCompletionEvent()
{
	return m_completeEvent;
}

ClientCallbackInterface *IocpClientJob::GetCallBackObject()
{
	return m_callBackObj;
}