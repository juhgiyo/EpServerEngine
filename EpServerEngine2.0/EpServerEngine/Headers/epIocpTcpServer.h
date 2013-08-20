/*! 
@file epIocpTcpServer.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief IOCP TCP Server Interface
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

An Interface for IOCP TCP Server.

*/

#ifndef __EP_IOCP_TCP_SERVER_H__
#define __EP_IOCP_TCP_SERVER_H__

#include "epServerEngine.h"
#include "epBaseTcpServer.h"

namespace epse{
		/*! 
	@class IocpTcpServer epIocpTcpServer.h
	@brief A class for IOCP TCP Server.
	*/
	class EP_SERVER_ENGINE IocpTcpServer:public BaseTcpServer, public WorkerThreadDelegate{
		public:
		/*!
		Default Constructor

		Initializes the Server
		@param[in] lockPolicyType The lock policy
		*/
		IocpTcpServer(epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);


		/*!
		Default Copy Constructor

		Initializes the Server
		@param[in] b the second object
		*/
		IocpTcpServer(const IocpTcpServer& b);
		/*!
		Default Destructor

		Destroy the Server
		*/
		virtual ~IocpTcpServer();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		IocpTcpServer & operator=(const IocpTcpServer&b);

		/*!
		Start the server
		@param[in] ops the server options
		@return true if successfully started otherwise false
		@remark if argument is NULL then previously setting value is used
		*/
		virtual bool StartServer(const ServerOps &ops=ServerOps::defaultServerOps);

		/*!
		Stop the server
		*/
		virtual void StopServer();
	private:

			
		/*!
		Call Back Function.
		@param[in] p the argument for call back function.
		*/
		virtual void CallBackFunc(BaseWorkerThread *p);

		friend class IocpTcpSocket;

		/*!
		Add new job to the worker thread.
		@param[in] job the job to push to the worker thread.
		*/
		void pushJob(BaseJob * job);

		/*!
		Listening Loop Function
		*/
		virtual void execute() ;

		/// general lock 
		epl::BaseLock *m_workerLock;

		/// Worker thread list
		vector<BaseWorkerThread*> m_workerList;
		/// worker thread list with no job
		queue<BaseWorkerThread*> m_emptyWorkerList;

	};
}

#endif //__EP_IOCP_TCP_SERVER_H__