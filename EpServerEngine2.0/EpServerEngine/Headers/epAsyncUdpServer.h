/*! 
@file epAsyncUdpServer.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Asynchronous UDP Server Interface
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

An Interface for Asynchronous UDP Server.

*/
#ifndef __EP_ASYNC_UDP_SERVER_H__
#define __EP_ASYNC_UDP_SERVER_H__


#include "epServerEngine.h"
#include "epBaseUdpServer.h"

namespace epse{

	/*! 
	@class AsyncUdpServer epAsyncUdpServer.h
	@brief A class for Asynchronous UDP Server.
	*/
	class EP_SERVER_ENGINE AsyncUdpServer:public BaseUdpServer{
		friend class AsyncUdpSocket;
	public:
		
		/*!
		Default Constructor

		Initializes the Server
		@param[in] callBackObj the callback object
		@param[in] port the port string
		@param[in] isAsynchronousReceive the flag for Asynchronous Receive
		@param[in] waitTimeMilliSec wait time for Server Thread to terminate
		@param[in] maximumConnectionCount the maximum number of connection
		@param[in] lockPolicyType The lock policy
		*/
		AsyncUdpServer(ServerCallbackInterface *callBackObj,const TCHAR * port=_T(DEFAULT_PORT),bool isAsynchronousReceive=true,unsigned int waitTimeMilliSec=WAITTIME_INIFINITE, unsigned int maximumConnectionCount=CONNECTION_LIMIT_INFINITE, epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the BaseServer
		@param[in] b the second object
		*/
		AsyncUdpServer(const AsyncUdpServer& b);
		/*!
		Default Destructor

		Destroy the Server
		*/
		virtual ~AsyncUdpServer();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		AsyncUdpServer & operator=(const AsyncUdpServer&b);
		
		/*!
		Get the asynchronous receive flag for the Socket.
		@return The flag whether to receive asynchronously.
		*/
		bool GetIsAsynchronousReceive() const;

		/*!
		Set the asynchronous receive flag for the Socket.
		@param[in] isASynchronousReceive The flag whether to receive asynchronously.
		*/
		void SetIsAsynchronousReceive(bool isASynchronousReceive);
	
	private:
	
		/*!
		Listening Loop Function
		*/
		virtual void execute();
	
		/// Flag for Asynchronous Receive
		bool m_isAsynchronousReceive;
	
	};
}
#endif //__EP_ASYNC_UDP_SERVER_H__