/*! 
@file epIocpServerJob.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief IOCP Server Job Interface
@version 1.0

@section LICENSE

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