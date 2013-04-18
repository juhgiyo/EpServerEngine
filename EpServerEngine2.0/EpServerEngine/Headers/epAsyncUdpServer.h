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
		@param[in] lockPolicyType The lock policy
		*/
		AsyncUdpServer(epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the Server
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

		
		/*!
		Start the server
		@param[in] ops the server options
		@remark if argument is NULL then previously setting value is used
		*/
		bool StartServer(const ServerOps &ops=ServerOps::defaultServerOps);
	
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