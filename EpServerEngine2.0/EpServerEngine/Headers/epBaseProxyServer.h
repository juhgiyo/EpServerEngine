/*! 
@file epBaseProxyServer.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Proxy Base Server Interface
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

An Interface for Proxy Base Server.

*/

#ifndef __EP_BASE_PROXY_SERVER_H__
#define __EP_BASE_PROXY_SERVER_H__

#include "epServerEngine.h"
#include "epProxyServerInterfaces.h"
#include "epBaseServer.h"
#include "epBaseProxyHandler.h"


namespace epse{

	
	/*! 
	@class BaseProxyServer epBaseProxyServer.h
	@brief A class for Proxy Base Server.
	*/
	class EP_SERVER_ENGINE BaseProxyServer:public ProxyServerInterface, public ServerCallbackInterface, public ClientCallbackInterface{
	public:

		/*!
		Default Constructor

		Initializes the Server
		@param[in] lockPolicyType The lock policy
		*/
		BaseProxyServer(epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the Server
		@param[in] b the second object
		*/
		BaseProxyServer(const BaseProxyServer& b);

		/*!
		Default Destructor

		Destroy the Server
		*/
		virtual ~BaseProxyServer();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseProxyServer & operator=(const BaseProxyServer&b);


		/*!
		Set the port for the server.
		@remark Cannot be changed while connected to server
		@param[in] port The port to set.
		*/
		void SetPort(const TCHAR *  port);

		/*!
		Get the port number of server
		@return the port number in string
		*/
		epl::EpTString GetPort() const;

		/*!
		Set the Maximum Connection Count for the server.
		@param[in] maxConnectionCount The Maximum Connection Count to set.
		@remark 0 means there is no limit
		*/
		void SetMaximumConnectionCount(unsigned int maxConnectionCount);

		/*!
		Get the Maximum Connection Count of server
		@return the Maximum Connection Count
		@remark 0 means there is no limit
		*/
		unsigned int GetMaximumConnectionCount() const;


		/*!
		Start the server
		@param[in] ops the proxy server options
		@return true if successfully started otherwise false
		@remark if argument is NULL then previously setting value is used
		*/
		bool StartServer(const ProxyServerOps &ops=ProxyServerOps::defaultProxyServerOps);

		/*!
		Stop the server
		*/
		void StopServer();

		/*!
		Check if the server is started
		@return true if the server is started otherwise false
		*/
		bool IsServerStarted() const;

		/*!
		Terminate all clients' socket connected.
		*/
		void ShutdownAllClient();

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
		/*!
		When new client tries to connect.
		@param[in] sockAddr the client's socket address which tries to connect
		@return true to accept the connection otherwise false.		
		*/
		bool OnAccept(sockaddr sockAddr);

		/*!
		When accepted client tries to make connection.
		@param[in] socket the client socket
		@remark When this function calls, it is right before making connection,<br/>
		        so user can configure the socket before the connection is actually made.		
		*/
		virtual void OnNewConnection(SocketInterface *socket)=0;

	

	protected:
		/// Proxy Server
		BaseServer *m_proxyServer;

		/// Proxy Handler List
		vector<BaseProxyHandler*> m_proxyHandlerList;

		/// Callback Object
		ProxyServerCallbackInterface *m_callBack;

		/// general lock 
		epl::BaseLock *m_baseProxyServerLock;

		/// Lock Policy
		epl::LockPolicy m_lockPolicy;

};
}


#endif //__EP_BASE_PROXY_SERVER_H__