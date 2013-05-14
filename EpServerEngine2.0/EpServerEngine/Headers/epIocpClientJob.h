/*! 
@file epIocpClientJob.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief IOCP Client Job Interface
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

An Interface for IOCP Client Job.

*/
#ifndef __EP_IOCP_CLIENT_JOB_H__
#define __EP_IOCP_CLIENT_JOB_H__

#include "epServerEngine.h"
#include "epBaseSocket.h"
#include "epClientInterfaces.h"

namespace epse{
		/*! 
	@class IocpClientJob epIocpClientJob.h
	@brief A class for IOCP Client Job.
	*/
	class EP_SERVER_ENGINE IocpClientJob:public BaseJob{

	public:
		/// Enumerator for client job type
		typedef enum _IocpClientJobType{
			/// VOID job
			IOCP_CLIENT_JOB_TYPE_NULL=0,
			/// send job
			IOCP_CLIENT_JOB_TYPE_SEND,
			/// receive job
			IOCP_CLIENT_JOB_TYPE_RECEIVE,
		}IocpClientJobType;

		/*!
		Default Constructor

		Initializes the Job
		@param[in] client the client which the job targeted
		@param[in] jobType the type of the job
		@param[in] packet the packet to do job
		@param[in] completionEvent the event for IO completion
		@param[in] callBackObj the callback object for IO completion
		@param[in] priority the priority of the job
		@param[in] lockPolicyType The lock policy
		*/
		IocpClientJob(BaseClient *client,IocpClientJobType jobType=IOCP_CLIENT_JOB_TYPE_NULL,Packet *packet=NULL,EventEx *completionEvent=NULL,ClientCallbackInterface *callBackObj=NULL,Priority priority=PRIORITY_NORMAL,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Destructor

		Destroy the Job
		*/
		virtual ~IocpClientJob();

		/*!
		Set actual job for this object
		@param[in] client the client which the job targeted
		@param[in] jobType the type of the job
		@param[in] packet the pointer to the packet object
		@param[in] completionEvent the event to set when the job is completed
		@param[in] callBackObj the callback object to invoke when the job is completed
		@remark if completionEvent of callBackObj is NULL the callback will be ignored!
		*/
		void SetJob(BaseClient *client,IocpClientJobType jobType,Packet *packet=NULL,EventEx *completionEvent=NULL,ClientCallbackInterface *callBackObj=NULL);

		/*!
		Return the type of job of this object
		@return the type of job
		*/
		IocpClientJobType GetJobType() const;

		/*!
		Return the packet set for this object
		@return the pointer to the packet
		*/
		const Packet *GetPacket() const;

		/*!
		Return the client set for this object
		@return the pointer to the client
		*/
		BaseClient *GetClient();

		/*!
		Return the event set for this object
		@return the pointer to the event
		*/
		EventEx *GetCompletionEvent();

		/*!
		Return the callback object set for this object
		@return the pointer to the callback object
		*/
		ClientCallbackInterface *GetCallBackObject();


	protected:
		/// pointer to the packet
		Packet *m_packet;

		/// job type
		IocpClientJobType m_jobType;

		/// event for job completion
		EventEx *m_completeEvent;

		/// callback object for job completion
		ClientCallbackInterface *m_callBackObj;

		/// the client object
		BaseClient *m_client;

	};
}


#endif //__EP_IOCP_CLIENT_JOB_H__