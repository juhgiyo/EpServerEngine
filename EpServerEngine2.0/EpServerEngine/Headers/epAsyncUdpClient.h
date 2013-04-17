/*! 
@file epAsyncUdpClient.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 18, 2012
@brief Asynchronous UDP Client Interface
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

An Interface for Asynchronous UDP Client.

*/
#ifndef __EP_ASYNC_CLIENT_UDP_H__
#define __EP_ASYNC_CLIENT_UDP_H__

#include "epServerEngine.h"
#include "epBaseUdpClient.h"
#include "epServerObjectList.h"
#include "epClientPacketProcessor.h"


namespace epse{

	/*! 
	@class AsyncUdpClient epAsyncUdpClient.h
	@brief A class for Asynchronous UDP Client.
	*/
	class EP_SERVER_ENGINE AsyncUdpClient:public BaseUdpClient{

	public:
		/*!
		Default Constructor

		Initializes the Client
		@param[in] callBackObj the call back object
		@param[in] hostName the hostname string
		@param[in] port the port string
		@param[in] isAsynchronousReceive the flag for asynchronous receive
		@param[in] waitTimeMilliSec wait time for Client Thread to terminate
		@param[in] maximumProcessorCount the maximum number of processor
		@param[in] lockPolicyType The lock policy
		*/
		AsyncUdpClient(ClientCallbackInterface *callBackObj,const TCHAR * hostName=_T(DEFAULT_HOSTNAME), const TCHAR * port=_T(DEFAULT_PORT),bool isAsynchronousReceive=true,unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,unsigned int maximumProcessorCount=PROCESSOR_LIMIT_INFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the Client
		@param[in] b the second object
		*/
		AsyncUdpClient(const AsyncUdpClient& b);
		/*!
		Default Destructor

		Destroy the Client
		*/
		virtual ~AsyncUdpClient();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		AsyncUdpClient & operator=(const AsyncUdpClient&b);
		

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
		@param[in] hostName the hostname string
		@param[in] port the port string
		@remark if argument is NULL then previously setting value is used
		*/
		bool Connect(const TCHAR * hostName=NULL, const TCHAR * port=NULL);

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
		@remark  Subclasses must implement this
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


#endif //__EP_ASYNC_CLIENT_UDP_H__