/*! 
@file epProxyTcpServer.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Proxy TCP Server Interface
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

An Interface for Proxy TCP Server.

*/

#ifndef __EP_PROXY_TCP_SERVER_H__
#define __EP_PROXY_TCP_SERVER_H__

#include "epServerEngine.h"
#include "epBaseProxyServer.h"
#include "epAsyncTcpServer.h"


namespace epse{

	
	/*! 
	@class ProxyTcpServer epProxyTcpServer.h
	@brief A class for Proxy TCP Server.
	*/
	class EP_SERVER_ENGINE ProxyTcpServer:public BaseProxyServer{
	public:

		/*!
		Default Constructor

		Initializes the Server
		@param[in] lockPolicyType The lock policy
		*/
		ProxyTcpServer(epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the Server
		@param[in] b the second object
		*/
		ProxyTcpServer(const ProxyTcpServer& b);

		/*!
		Default Destructor

		Destroy the Server
		*/
		virtual ~ProxyTcpServer();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		ProxyTcpServer & operator=(const ProxyTcpServer&b);

	
	private:
		/*!
		When accepted client tries to make connection.
		@param[in] socket the client socket
		@remark When this function calls, it is right before making connection,<br/>
		        so user can configure the socket before the connection is actually made.		
		*/
		virtual void OnNewConnection(SocketInterface *socket);

	};
}


#endif //__EP_PROXY_TCP_SERVER_H__