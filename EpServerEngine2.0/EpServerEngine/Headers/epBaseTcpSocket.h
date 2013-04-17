/*! 
@file epBaseTcpSocket.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base TCP Socket Interface
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
		@return sent byte size
		@remark return -1 if error occurred
		*/
		int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE);
		

		/*!
		Kill the connection
		*/
		void KillConnection()=0;
		
	
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