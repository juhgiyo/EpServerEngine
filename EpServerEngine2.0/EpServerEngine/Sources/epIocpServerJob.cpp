/*! 
IocpServerJob for the EpServerEngine
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