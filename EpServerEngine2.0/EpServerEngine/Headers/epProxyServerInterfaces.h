/*! 
@file epProxyServerInterfaces.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Proxy Server Interfaces
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

An Interface for Proxy Server Interfaces.

*/
#ifndef __EP_PROXY_SERVER_INTERFACES_H__
#define __EP_PROXY_SERVER_INTERFACES_H__
#include "epServerEngine.h"
#include "epServerInterfaces.h"
#include "epClientInterfaces.h"
namespace epse{

	class ProxyServerCallbackInterface;

	/*! 
	@class ForwardServerInfo epProxyServerInterfaces.h
	@brief A class for Forward Server Info Interface.
	*/
	struct ForwardServerInfo{
		/// Hostname
		const TCHAR *hostname;
		/// Port
		const TCHAR *port;

		/*!
		Default Constructor

		Initializes the Forward Server Options
		*/
		ForwardServerInfo()
		{
			hostname=_T(DEFAULT_HOSTNAME);
			port=_T(DEFAULT_PORT);
		}

		/// Default Forward Server Info
		static ForwardServerInfo defaultForwardServerInfo;
	};

	/*! 
	@struct ProxyServerOps epProxyServerInterfaces.h
	@brief A class for Proxy Server Options.
	*/
	struct EP_SERVER_ENGINE ProxyServerOps{
		/// Callback Object
		ProxyServerCallbackInterface *callBackObj;
		/// Port
		const TCHAR *port;
		///The maximum possible number of client connection
		unsigned int maximumConnectionCount;

		/*!
		Default Constructor

		Initializes the Proxy Server Options
		*/
		ProxyServerOps()
		{
			callBackObj=NULL;
			port=_T(DEFAULT_PORT);
			maximumConnectionCount=CONNECTION_LIMIT_INFINITE;
		}

		/// Default Proxy Server Options
		static ProxyServerOps defaultProxyServerOps;
	};

	
	/*! 
	@class ProxyServerInterface epProxyServerInterfaces.h
	@brief A class for Proxy Server Interface.
	*/
	class EP_SERVER_ENGINE ProxyServerInterface{
		/*!
		Set the port for the server.
		@remark Cannot be changed while connected to server
		@param[in] port The port to set.
		*/
		virtual void SetPort(const TCHAR *  port)=0;

		/*!
		Get the port number of server
		@return the port number in string
		*/
		virtual epl::EpTString GetPort() const=0;

		/*!
		Set the Maximum Connection Count for the server.
		@param[in] maxConnectionCount The Maximum Connection Count to set.
		@remark 0 means there is no limit
		*/
		virtual void SetMaximumConnectionCount(unsigned int maxConnectionCount)=0;

		/*!
		Get the Maximum Connection Count of server
		@return the Maximum Connection Count
		@remark 0 means there is no limit
		*/
		virtual unsigned int GetMaximumConnectionCount() const=0;


		/*!
		Start the server
		@param[in] ops the proxy server options
		@remark if argument is NULL then previously setting value is used
		*/
		virtual bool StartServer(const ProxyServerOps &ops=ProxyServerOps::defaultProxyServerOps)=0;

		/*!
		Stop the server
		*/
		virtual void StopServer()=0;

		/*!
		Check if the server is started
		@return true if the server is started otherwise false
		*/
		virtual bool IsServerStarted() const=0;

		/*!
		Terminate all clients' socket connected.
		*/
		virtual void ShutdownAllClient()=0;

		/*!
		Set the Callback Object for the server.
		@param[in] callBackObj The Callback Object to set.
		*/
		virtual void SetCallbackObject(ProxyServerCallbackInterface *callBackObj)=0;

		/*!
		Get the Callback Object of server
		@return the current Callback Object
		*/
		virtual ProxyServerCallbackInterface *GetCallbackObject()=0;
	};

	/*! 
	@class ProxyServerCallbackInterface epProxyServerInterfaces.h
	@brief A class for Proxy Server Callback Interface.
	*/
	class EP_SERVER_ENGINE ProxyServerCallbackInterface{
	public:
		/*!
		When new client tries to connect.
		@param[in] sockAddr the client's socket address which tries to connect
		@return true to accept the connection otherwise false.		
		*/
		virtual bool OnAccept(sockaddr sockAddr)
		{
			return true;
		}

		/*!
		Received the packet from the client.
		@param[in] clientSocket the client socket which the packet is received from
		@param[in] forwardServerClient the connected server for forwarding
		@param[in] receivedPacket the received packet
		@remark The default is just forwarding the packet to forward server.
		*/
		virtual void OnReceivedFromClient(SocketInterface *clientSocket,ClientInterface *forwardServerClient, const Packet&receivedPacket)
		{
			forwardServerClient->Send(receivedPacket);
		}

		/*!
		Received the packet from the forward server.
		@param[in] clientSocket the client socket
		@param[in] forwardServerClient the forward server which the packet is received from
		@param[in] receivedPacket the received packet
		@remark The default is just forwarding the packet to the client.
		*/
		virtual void OnReceivedFromForwardServer(SocketInterface *clientSocket,ClientInterface *forwardServerClient, const Packet&receivedPacket)
		{
			clientSocket->Send(receivedPacket);
		}

		/*!
		The client is disconnected.
		@param[in] sockAddr the disconnected client's sockaddr.
		*/
		virtual void OnDisconnect(sockaddr sockAddr){}

		/*!
		Get the forward server info
		@param[in] socketAddr the sockaddr of the client which tries to connect
		@return the forward server info
		@remark the subclass must specify the forward server info!
		*/
		virtual ForwardServerInfo GetForwardServerInfo(const sockaddr &socketAddr)=0;



	};


}

#endif //__EP_PROXY_SERVER_INTERFACES_H__