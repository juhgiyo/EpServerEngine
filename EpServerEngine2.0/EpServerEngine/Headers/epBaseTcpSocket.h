/*! 
@file epBaseTcpSocket.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base TCP Socket Interface
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

An Interface for Base TCP Socket.

*/
#ifndef __EP_BASE_TCP_SOCKET_H__
#define __EP_BASE_TCP_SOCKET_H__

#include "epServerEngine.h"
#include "epBaseSocket.h"

namespace epse
{

	/*! 
	@class BaseTcpSocket epBaseTcpSocket.h
	@brief A class for Base TCP Socket.
	*/
	class EP_SERVER_ENGINE BaseTcpSocket:public BaseSocket
	{
	public:
		/*!
		Default Constructor

		Initializes the Socket
		@param[in] callBackObj the callback object
		@param[in] waitTimeMilliSec wait time for Socket Thread to terminate
		@param[in] lockPolicyType The lock policy
		*/
		BaseTcpSocket(ServerCallbackInterface *callBackObj,unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Destructor

		Destroy the Socket
		*/
		virtual ~BaseTcpSocket();

		/*!
		Send the packet to the client
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@param[in] sendStatus the status of Send
		@return sent byte size
		@remark return -1 if error occurred
		*/
		virtual int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE,SendStatus *sendStatus=NULL);
		

		/*!
		Kill the connection
		*/
		virtual void KillConnection()=0;
		

	protected:	
		friend class SyncTcpServer;
		friend class AsyncTcpServer;
		/*!
		Actually Kill the connection
		*/
		virtual void killConnection()=0;

		/*!
		thread loop function
		*/
		virtual void execute()=0;
		
		/*!
		Receive the packet from the client
		@remark  Subclasses must implement this
		@param[out] packet the packet received
		@return received byte size
		*/
		int receive(Packet &packet);
	
		/*!
		Set the argument for the base server worker thread.
		@param[in] clientSocket The client socket from server.
		*/
		void setClientSocket(const SOCKET& clientSocket );



	protected:
		/*!
		Default Copy Constructor

		Initializes the Socket
		@param[in] b the second object
		@remark Copy Constructor prohibited
		*/
		BaseTcpSocket(const BaseTcpSocket& b):BaseSocket(b)
		{}

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		@remark Copy Operator prohibited
		*/
		BaseTcpSocket & operator=(const BaseTcpSocket&b){return *this;}
	
	
	protected:

		/// client socket
		SOCKET m_clientSocket;

		/// send lock
		epl::BaseLock *m_sendLock;

		/// Temp Packet;
		Packet m_recvSizePacket;
	};

}

#endif //__EP_BASE_TCP_SOCKET_H__