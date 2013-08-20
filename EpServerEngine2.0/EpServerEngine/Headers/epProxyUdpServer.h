/*! 
@file epProxyUdpServer.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Proxy UDP Server Interface
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

An Interface for Proxy UDP Server.

*/

#ifndef __EP_PROXY_UDP_SERVER_H__
#define __EP_PROXY_UDP_SERVER_H__

#include "epServerEngine.h"
#include "epBaseProxyServer.h"


namespace epse{

	
	/*! 
	@class ProxyUdpServer epProxyUdpServer.h
	@brief A class for Proxy UDP Server.
	*/
	class EP_SERVER_ENGINE ProxyUdpServer:public BaseProxyServer{
	public:

		/*!
		Default Constructor

		Initializes the Server
		@param[in] lockPolicyType The lock policy
		*/
		ProxyUdpServer(epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the Server
		@param[in] b the second object
		*/
		ProxyUdpServer(const ProxyUdpServer& b);

		/*!
		Default Destructor

		Destroy the Server
		*/
		virtual ~ProxyUdpServer();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		ProxyUdpServer & operator=(const ProxyUdpServer&b);

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


#endif //__EP_PROXY_UDP_SERVER_H__