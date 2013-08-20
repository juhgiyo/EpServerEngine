/*! 
@file epProxyServerInterfaces.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Proxy Server Interfaces
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
		@param[in] ops the server options
		@return true if successfully started otherwise false
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
		virtual void OnReceivedFromClient(SocketInterface *clientSocket,ClientInterface *forwardServerClient, const Packet*receivedPacket)
		{
			if(receivedPacket)
				forwardServerClient->Send(*receivedPacket);
		}

		/*!
		Received the packet from the forward server.
		@param[in] clientSocket the client socket
		@param[in] forwardServerClient the forward server which the packet is received from
		@param[in] receivedPacket the received packet
		@remark The default is just forwarding the packet to the client.
		*/
		virtual void OnReceivedFromForwardServer(SocketInterface *clientSocket,ClientInterface *forwardServerClient, const Packet*receivedPacket)
		{
			if(receivedPacket)
				clientSocket->Send(*receivedPacket);
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