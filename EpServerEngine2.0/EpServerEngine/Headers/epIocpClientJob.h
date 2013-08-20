/*! 
@file epIocpClientJob.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief IOCP Client Job Interface
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