/*! 
@file epBaseProxyHandler.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Proxy Base Handler Interface
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

An Interface for Proxy Base Handler.

*/

#ifndef __EP_BASE_PROXY_HANDLE_H__
#define __EP_BASE_PROXY_HANDLE_H__
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
		@param[in] socket the client socket
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
		@param[in] status the status of Receive
		@remark for Asynchronous Server Use Only!
		*/
		void OnReceived(SocketInterface *socket,const Packet *receivedPacket,ReceiveStatus status);

		/*!
		The client is disconnected.
		@param[in] socket the client socket, disconnected.
		*/
		void OnDisconnect(SocketInterface *socket);


		/*!
		Received the packet from the server.
		@param[in] client the client which received the packet
		@param[in] receivedPacket the received packet
		@param[in] status the status of Receive
		@remark for Asynchronous Client Use Only!
		*/
		void OnReceived(ClientInterface *client,const Packet*receivedPacket,ReceiveStatus status);

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

		/*!
		Get the IP of client
		@return the IP of client in string
		*/
		epl::EpTString GetIP() const;

		/*!
		Get the sockaddr of client
		@return the sockaddr of client
		*/
		sockaddr GetSockAddress() const;

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

#endif //__EP_BASE_PROXY_HANDLE_H__