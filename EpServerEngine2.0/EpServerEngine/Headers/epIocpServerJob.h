/*! 
@file epIocpServerJob.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief IOCP Server Job Interface
@version 1.0

@section LICENSE

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

@section DESCRIPTION

An Interface for IOCP Server Job.

*/
#ifndef __EP_IOCP_SERVER_JOB_H__
#define __EP_IOCP_SERVER_JOB_H__

#include "epServerEngine.h"
#include "epBaseSocket.h"

namespace epse{
		/*! 
	@class IocpServerJob epIocpServerJob.h
	@brief A class for IOCP SERVER Job.
	*/
	class EP_SERVER_ENGINE IocpServerJob:public BaseJob{

	public:

		typedef enum _IocpServerJobType{
			IOCP_SERVER_JOB_TYPE_NULL=0,
			IOCP_SERVER_JOB_TYPE_SEND,
			IOCP_SERVER_JOB_TYPE_RECEIVE,
			IOCP_SERVER_JOB_TYPE_DISCONNECT,
		}IocpServerJobType;

		/*!
		Default Constructor

		Initializes the Job
		@param[in] socket the client socket to do the job
		@param[in] packet the packet to do job
		@param[in] priority the priority of the job
		@param[in] lockPolicyType The lock policy
		*/
		IocpServerJob(BaseSocket *socket=NULL,IocpServerJobType jobType=IOCP_SERVER_JOB_TYPE_NULL,Packet *packet=NULL,EventEx *completionEvent=NULL,ServerCallbackInterface *callBackObj=NULL,Priority priority=PRIORITY_NORMAL,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Destructor

		Destroy the Socket
		*/
		virtual ~IocpServerJob();

		void SetJob(BaseSocket *socket,IocpServerJobType jobType,Packet *packet=NULL,EventEx *completionEvent=NULL,ServerCallbackInterface *callBackObj=NULL);

		IocpServerJobType GetJobType() const;

		const Packet *GetPacket() const;

		BaseSocket *GetSocket();

		EventEx *GetCompletionEvent();

		ServerCallbackInterface *GetCallBackObject();


	protected:
		Packet *m_packet;

		IocpServerJobType m_jobType;

		BaseSocket *m_socket;

		EventEx *m_completeEvent;

		ServerCallbackInterface *m_callBackObj;

	};
}


#endif //__EP_IOCP_SERVER_JOB_H__