/*! 
@file epBaseProxyServer.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Proxy Base Server Interface
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

An Interface for Proxy Base Server.

*/

#ifndef __EP_PROXY_BASE_SERVER_H__
#define __EP_PROXY_BASE_SERVER_H__

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


#endif //__EP_PROXY_SERVER_H__