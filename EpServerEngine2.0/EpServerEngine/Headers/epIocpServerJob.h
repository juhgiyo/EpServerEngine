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
		/// Enumerator for server job type
		typedef enum _IocpServerJobType{
			/// VOID job
			IOCP_SERVER_JOB_TYPE_NULL=0,
			/// send job
			IOCP_SERVER_JOB_TYPE_SEND,
			/// receive job
			IOCP_SERVER_JOB_TYPE_RECEIVE,
			/// disconnect job
			IOCP_SERVER_JOB_TYPE_DISCONNECT,
		}IocpServerJobType;

		/*!
		Default Constructor

		Initializes the Job
		@param[in] socket the client socket to do the job
		@param[in] jobType the type of the job
		@param[in] packet the packet to do job
		@param[in] completionEvent the event for IO completion
		@param[in] callBackObj the callback object for IO completion
		@param[in] priority the priority of the job
		@param[in] lockPolicyType The lock policy
		*/
		IocpServerJob(BaseSocket *socket=NULL,IocpServerJobType jobType=IOCP_SERVER_JOB_TYPE_NULL,Packet *packet=NULL,EventEx *completionEvent=NULL,ServerCallbackInterface *callBackObj=NULL,Priority priority=PRIORITY_NORMAL,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Destructor

		Destroy the Socket
		*/
		virtual ~IocpServerJob();

		/*!
		Set actual job for this object
		@param[in] socket the socket which the job targeted
		@param[in] jobType the type of the job
		@param[in] packet the pointer to the packet object
		@param[in] completionEvent the event to set when the job is completed
		@param[in] callBackObj the callback object to invoke when the job is completed
		@remark if completionEvent of callBackObj is NULL the callback will be ignored!
		*/
		void SetJob(BaseSocket *socket,IocpServerJobType jobType,Packet *packet=NULL,EventEx *completionEvent=NULL,ServerCallbackInterface *callBackObj=NULL);

		/*!
		Return the type of job of this object
		@return the type of job
		*/
		IocpServerJobType GetJobType() const;

		/*!
		Return the packet set for this object
		@return the pointer to the packet
		*/
		const Packet *GetPacket() const;

		/*!
		Return the socket set for this object
		@return the pointer to the socket
		*/
		BaseSocket *GetSocket();

		/*!
		Return the event set for this object
		@return the pointer to the event
		*/
		EventEx *GetCompletionEvent();

		/*!
		Return the callback object set for this object
		@return the pointer to the callback object
		*/
		ServerCallbackInterface *GetCallBackObject();


	protected:
		/// pointer to the packet
		Packet *m_packet;

		/// job type
		IocpServerJobType m_jobType;

		/// pointer to the socket
		BaseSocket *m_socket;

		/// event for job completion
		EventEx *m_completeEvent;

		/// callback object for job completion
		ServerCallbackInterface *m_callBackObj;

	};
}


#endif //__EP_IOCP_SERVER_JOB_H__