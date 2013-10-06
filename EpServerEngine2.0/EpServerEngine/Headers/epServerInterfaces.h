/*! 
@file epServerInterfaces.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Server Interfaces
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

An Interface for Server Interfaces.

*/
#ifndef __EP_SERVER_INTERFACES_H__
#define __EP_SERVER_INTERFACES_H__

#include "epServerEngine.h"
#include <winsock2.h>
#include "epPacket.h"
#include "epBaseServerObject.h"
namespace epse{
	class ServerCallbackInterface;

	/*! 
	@struct ServerOps epServerInterfaces.h
	@brief A class for Server Options.
	*/
	struct EP_SERVER_ENGINE ServerOps{
		/// Callback Object
		ServerCallbackInterface *callBackObj;
		/// Port
		const TCHAR *port;
		/*!
		The flag for asynchronous receive.
		@remark For Asynchronous Client Use Only!
		*/
		bool isAsynchronousReceive;
		/// Wait time in millisecond for client threads
		unsigned int waitTimeMilliSec;
		///The maximum possible number of client connection
		unsigned int maximumConnectionCount;

		/*!
		The number of worker thread.
		@remark For IOCP Use Only!
		*/
		unsigned int workerThreadCount;

		/*!
		Default Constructor

		Initializes the Server Options
		*/
		ServerOps()
		{
			callBackObj=NULL;
			port=_T(DEFAULT_PORT);
			isAsynchronousReceive=true;
			waitTimeMilliSec=WAITTIME_INIFINITE;
			maximumConnectionCount=CONNECTION_LIMIT_INFINITE;
			workerThreadCount=0;

		}

		static ServerOps defaultServerOps;
	};

	/*! 
	@class ServerInterface epServerInterfaces.h
	@brief A class for Server Interface.
	*/
	class EP_SERVER_ENGINE ServerInterface{
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
		Set the Callback Object for the server.
		@param[in] callBackObj The Callback Object to set.
		*/
		virtual void SetCallbackObject(ServerCallbackInterface *callBackObj)=0;

		/*!
		Get the Callback Object of server
		@return the current Callback Object
		*/
		virtual ServerCallbackInterface *GetCallbackObject()=0;

		/*!
		Start the server
		@param[in] ops the server options
		@return true if successfully started otherwise false
		@remark if argument is NULL then previously setting value is used
		*/
		virtual bool StartServer(const ServerOps &ops=ServerOps::defaultServerOps)=0;
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
		Get the maximum packet byte size
		@return the maximum packet byte size
		@remark for UDP Server Use Only!
		*/
		virtual unsigned int GetMaxPacketByteSize() const
		{
			return 0;
		}

		/*!
		Get the asynchronous receive flag for the Socket.
		@return The flag whether to receive asynchronously.
		@remark for Asynchronous Server Use Only!
		*/
		virtual bool GetIsAsynchronousReceive() const
		{
			return false;
		}

		/*!
		Set the asynchronous receive flag for the Socket.
		@param[in] isASynchronousReceive The flag whether to receive asynchronously.
		@remark for Asynchronous Server Use Only!
		*/
		virtual void SetIsAsynchronousReceive(bool isASynchronousReceive)
		{
			return;
		}
	};


	/*! 
	@class SocketInterface epServerInterfaces.h
	@brief A class for Socket Interface.
	*/
	class EP_SERVER_ENGINE SocketInterface{
	public:

		/*!
		Send the packet to the client
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@param[in] sendStatus the status of Send
		@return sent byte size
		@remark return -1 if error occurred
		*/
		virtual int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE,SendStatus *sendStatus=NULL)=0;

		/*!
		Receive the packet from the client
		@param[in] waitTimeInMilliSec wait time for receiving the packet in millisecond
		@param[out] retStatus the pointer to ReceiveStatus enumerator to get receive status.
		@return received packet
		@remark the caller must call ReleaseObj() for Packet to avoid the memory leak.
		@remark For Synchronous Socket Use Only!
		*/
		virtual Packet *Receive(unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE,ReceiveStatus *retStatus=NULL)
		{
			if(retStatus)
				*retStatus=RECEIVE_STATUS_FAIL_NOT_SUPPORTED;
			return NULL;
		}

		/*!
		Check if the connection is alive
		@return true if the connection is alive otherwise false
		*/
		virtual bool IsConnectionAlive() const=0;

		/*!
		Kill the connection
		*/
		virtual void KillConnection()=0;

		/*!
		Get the sockaddr of this socket object.
		@return the sockaddr of this socket object.
		*/
		virtual sockaddr GetSockAddress() const=0;

		/*!
		Get the IP of client
		@return the IP of client in string
		*/
		virtual epl::EpTString GetIP() const=0;

		/*!
		Get the owner object of this socket object.
		@return the pointer to the owner object.
		*/
		virtual BaseServerObject *GetOwner()=0;
		
		/*!
		Set the wait time for the thread termination
		@param[in] milliSec the time for waiting in millisecond
		*/
		virtual void SetWaitTime(unsigned int milliSec)=0;
		
		/*!
		Get the wait time for the parser thread termination
		@return the current time for waiting in millisecond
		*/
		virtual unsigned int GetWaitTime() const=0;

		/*!
		Set the Callback Object for the server.
		@param[in] callBackObj The Callback Object to set.
		*/
		virtual void SetCallbackObject(ServerCallbackInterface *callBackObj)=0;

		/*!
		Get the Callback Object of server
		@return the current Callback Object
		*/
		virtual ServerCallbackInterface *GetCallbackObject()=0;

		/*!
		Set the Maximum Processor Count for the Socket.
		@param[in] maxProcessorCount The Maximum Processor Count to set.
		@remark 0 means there is no limit
		@remark For Asynchronous Socket Use Only!
		*/
		virtual void SetMaximumProcessorCount(unsigned int maxProcessorCount){}

		/*!
		Get the Maximum Processor Count of the Socket
		@return The Maximum Processor Count
		@remark 0 means there is no limit
		@remark For Asynchronous Socket Use Only!
		*/
		virtual unsigned int GetMaximumProcessorCount() const{return 0;}

		/*!
		Get the asynchronous receive flag for the Socket.
		@return The flag whether to receive asynchronously.
		@remark for Asynchronous Socket Use Only!
		*/
		virtual bool GetIsAsynchronousReceive() const
		{
			return false;
		}

		/*!
		Set the asynchronous receive flag for the Socket.
		@param[in] isASynchronousReceive The flag whether to receive asynchronously.
		@remark for Asynchronous Socket Use Only!
		*/
		virtual void SetIsAsynchronousReceive(bool isASynchronousReceive)
		{
			return;
		}

		/*!
		Get the maximum packet byte size
		@return the maximum packet byte size
		@remark for UDP Socket Use Only!
		*/
		virtual unsigned int GetMaxPacketByteSize() const{return 0;}

		

	};

	/*! 
	@class ServerCallbackInterface epServerInterfaces.h
	@brief A class for Server Callback Interface.
	*/
	class EP_SERVER_ENGINE ServerCallbackInterface{
	public:
		/*!
		When new client tries to connect.
		@param[in] sockAddr the client's socket address which tries to connect
		@return true to accept the connection otherwise false.		
		*/
		virtual bool OnAccept(sockaddr sockAddr){return true;}

		/*!
		When accepted client tries to make connection.
		@param[in] socket the client socket
		@remark When this function calls, it is right before making connection,<br/>
		        so user can configure the socket before the connection is actually made.		
		*/
		virtual void OnNewConnection(SocketInterface *socket){}

		/*!
		Received the packet from the client.
		@param[in] socket the client socket which received the packet
		@param[in] receivedPacket the received packet
		@param[in] status the status of Receive
		@remark for Asynchronous Server Use Only!
		*/
		virtual void OnReceived(SocketInterface *socket,const Packet*receivedPacket,ReceiveStatus status)=0;

		/*!
		Sent the packet from the client.
		@param[in] socket the client socket which sent the packet
		@param[in] status the status of Send
		@remark for IOCP Server Use Only!
		*/
		virtual void OnSent(SocketInterface *socket,SendStatus status){}

		/*!
		The client is disconnected.
		@param[in] socket the client socket, disconnected.
		*/
		virtual void OnDisconnect(SocketInterface *socket){}
	};

}

#endif //__EP_SERVER_INTERFACES_H__