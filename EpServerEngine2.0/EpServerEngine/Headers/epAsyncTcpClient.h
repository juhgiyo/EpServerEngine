/*! 
@file epAsyncTcpClient.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Asynchronous TCP Client Interface
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

An Interface for Asynchronous TCP Client.

*/
#ifndef __EP_ASYNC_TCP_CLIENT_H__
#define __EP_ASYNC_TCP_CLIENT_H__

#include "epServerEngine.h"
#include "epBaseTcpClient.h"
#include "epServerObjectList.h"

#include "epClientPacketProcessor.h"


namespace epse{

	/*! 
	@class AsyncTcpClient epAsyncTcpClient.h
	@brief A class for Asynchronous TCP Client.
	*/
	class EP_SERVER_ENGINE AsyncTcpClient:public BaseTcpClient{
	public:
		/*!
		Default Constructor

		Initializes the Client
		@param[in] lockPolicyType The lock policy
		*/
		AsyncTcpClient(epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the Client
		@param[in] b the second object
		*/
		AsyncTcpClient(const AsyncTcpClient& b);
		/*!
		Default Destructor

		Destroy the Client
		*/
		virtual ~AsyncTcpClient();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		AsyncTcpClient & operator=(const AsyncTcpClient&b);
	

		/*!
		Set the Maximum Processor Count for the server.
		@param[in] maxProcessorCount The Maximum Processor Count to set.
		@remark 0 means there is no limit
		*/
		void SetMaximumProcessorCount(unsigned int maxProcessorCount);

		/*!
		Get the Maximum Parser Parser of server
		@return The Maximum Connection Count
		@remark 0 means there is no limit
		*/
		unsigned int GetMaximumProcessorCount() const;


		/*!
		Set the wait time for the thread termination
		@param[in] milliSec the time for waiting in millisecond
		*/
		void SetWaitTime(unsigned int milliSec);

		/*!
		Connect to the server
		@param[in] ops the client options
		@return true if successfully connected otherwise false
		@remark if argument is NULL then previously setting value is used
		*/
		bool Connect(const ClientOps &ops=ClientOps::defaultClientOps);

		/*!
		Disconnect from the server
		*/
		void Disconnect();
	
		/*!
		Get the asynchronous receive flag for the Socket.
		@return The flag whether to receive asynchronously.
		@remark for Asynchronous Client Use Only!
		*/
		bool GetIsAsynchronousReceive() const;
		

		/*!
		Set the asynchronous receive flag for the Socket.
		@param[in] isASynchronousReceive The flag whether to receive asynchronously.
		@remark for Asynchronous Client Use Only!
		*/
		void SetIsAsynchronousReceive(bool isASynchronousReceive);

	private:


		/*!
		Actually processing the client thread
		*/
		virtual void execute();

		/*!
		Actually Disconnect from the server
		*/
		void disconnect();


	private:

		/// Processor list
		ServerObjectList m_processorList;

		/// Maximum Processor Count
		unsigned int m_maxProcessorCount;

		/// Flag for Asynchronous Receive
		bool m_isAsynchronousReceive;


	};
}


#endif //__EP_ASYNC_TCP_CLIENT_H__