/*! 
@file epBaseTcpServer.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base TCP Server Interface
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

An Interface for Base TCP Server.

*/
#ifndef __EP_BASE_TCP_SERVER_H__
#define __EP_BASE_TCP_SERVER_H__

#include "epServerEngine.h"
#include "epBaseServer.h"

namespace epse{


	/*! 
	@class BaseTcpServer epBaseTcpServer.h
	@brief A class for Base TCP Server.
	*/
	class EP_SERVER_ENGINE BaseTcpServer:public BaseServer
	{

	public:
		/*!
		Default Constructor

		Initializes the Server
		@param[in] lockPolicyType The lock policy
		*/
		BaseTcpServer( epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		
		/*!
		Default Copy Constructor

		Initializes the Server
		@param[in] b the second object
		*/
		BaseTcpServer(const BaseTcpServer& b);
		/*!
		Default Destructor

		Destroy the Server
		*/
		virtual ~BaseTcpServer();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseTcpServer & operator=(const BaseTcpServer&b);
				

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
		Listening Loop Function
		*/
		virtual void execute()=0;

	};
}
#endif //__EP_BASE_TCP_SERVER_H__