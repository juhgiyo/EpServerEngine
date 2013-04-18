/*! 
@file epBaseProxyHandler.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Proxy Base Handler Interface
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

An Interface for Proxy Base Handler.

*/

#ifndef __EP_PROXY_BASE_HANDLE_H__
#define __EP_PROXY_BASE_HANDLE_H__
#include "epServerEngine.h"
#include "epProxyServerInterfaces.h"
#include "epBaseClient.h"

namespace epse{


	/*! 
	@class BaseProxyHandler epBaseProxyHandler.h
	@brief A class for Proxy Base Handler.
	*/
	class BaseProxyHandler:public ServerCallbackInterface, public ClientCallbackInterface{

		friend class BaseProxyServer;
	protected:
		/*!
		Default Constructor

		Initializes the Handler
		@param[in] callBack the callback object
		@param[in] lockPolicyType The lock policy
		*/
		BaseProxyHandler(ProxyServerCallbackInterface *callBack, SocketInterface *socket, epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);


		/*!
		Default Destructor

		Destroy the Handler
		*/
		virtual ~BaseProxyHandler();

		/*!
		Received the packet from the client.
		@param[in] socket the client socket which received the packet
		@param[in] receivedPacket the received packet
		@remark for Asynchronous Server Use Only!
		*/
		void OnReceived(SocketInterface *socket,const Packet&receivedPacket);

		/*!
		The client is disconnected.
		@param[in] socket the client socket, disconnected.
		*/
		void OnDisconnect(SocketInterface *socket);


		/*!
		Received the packet from the server.
		@param[in] client the client which received the packet
		@param[in] receivedPacket the received packet
		@remark for Asynchronous Client Use Only!
		*/
		void OnReceived(ClientInterface *client,const Packet&receivedPacket);

		/*!
		The client is disconnected.
		@param[in] client the client, disconnected.
		*/
		void OnDisconnect(ClientInterface *client);

		/*!
		Set the Callback Object for the server.
		@param[in] callBackObj The Callback Object to set.
		*/
		void SetCallbackObject(ProxyServerCallbackInterface *callBackObj);

		/*!
		Get the Callback Object of server
		@return the current Callback Object
		*/
		ProxyServerCallbackInterface *GetCallbackObject();

	protected:
		/// client socket
		SocketInterface *m_client;
		/// the client connected to forward server
		BaseClient *m_forwardClient;
		/// callback object
		ProxyServerCallbackInterface *m_callBack;


		/// general lock 
		epl::BaseLock *m_baseProxyHandlerLock;

		/// Lock Policy
		epl::LockPolicy m_lockPolicy;
	};
}

#endif //__EP_PROXY_BASE_HANDLE_H__