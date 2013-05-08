/*! 
IocpClientJob for the EpServerEngine
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