/*! 
@file epProxyUdpHandler.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Proxy UDP Handler Interface
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

An Interface for Proxy UDP Handler.

*/

#ifndef __EP_PROXY_UDP_HANDLE_H__
#define __EP_PROXY_UDP_HANDLE_H__
#include "epServerEngine.h"
#include "epBaseProxyHandler.h"

namespace epse{


	/*! 
	@class ProxyUdpHandler epProxyUdpHandler.h
	@brief A class for Proxy UDP Handler.
	*/
	class ProxyUdpHandler:public BaseProxyHandler{

		friend class ProxyUdpServer;
	private:
		/*!
		Default Constructor

		Initializes the Handler
		@param[in] callBack the callback object
		@param[in] forwardServerInfo the forward server info
		@param[in] socket the client socket
		@param[in] lockPolicyType The lock policy
		*/
		ProxyUdpHandler(ProxyServerCallbackInterface *callBack,const ForwardServerInfo& forwardServerInfo, SocketInterface *socket, epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);


		/*!
		Default Destructor

		Destroy the Handler
		*/
		virtual ~ProxyUdpHandler();

		};
}

#endif //__EP_PROXY_UDP_HANDLE_H__