/*! 
@file epProxyUdpHandler.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Proxy UDP Handler Interface
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