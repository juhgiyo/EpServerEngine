/*! 
@file epBaseSocket.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base Socket Interface
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

An Interface for Base Socket.

*/
#ifndef __EP_BASE_SOCKET_H__
#define __EP_BASE_SOCKET_H__

#include "epServerEngine.h"
#include "epPacket.h"
#include "epServerPacketProcessor.h"
#include "epServerConf.h"
#include "epServerObjectList.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>


// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")


namespace epse
{

	/*! 
	@class BaseSocket epBaseSocket.h
	@brief A class for Base Socket.
	*/
	class EP_SERVER_ENGINE BaseSocket:public BaseServerObject,public SocketInterface
	{
	public:
		/*!
		Default Constructor

		Initializes the Socket
		@param[in] callBackObj the callback object
		@param[in] waitTimeMilliSec wait time for Worker Thread to terminate
		@param[in] lockPolicyType The lock policy
		*/
		BaseSocket(ServerCallbackInterface *callBackObj,unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Destructor

		Destroy the Socket
		*/
		virtual ~BaseSocket();

		/*!
		Send the packet to the client
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@return sent byte size
		@remark return -1 if error occurred
		*/
		virtual int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE)=0;

		
		/*!
		Check if the connection is alive
		@return true if the connection is alive otherwise false
		*/
		virtual bool IsConnectionAlive() const;

		/*!
		Kill the connection
		*/
		virtual void KillConnection()=0;
		
		/*!
		Get the sockaddr of this socket object.
		@return the sockaddr of this socket object.
		*/
		virtual sockaddr GetSockAddress() const;

		
		/*!
		Get the owner object of this socket object.
		@return the pointer to the owner object.
		*/
		virtual BaseServerObject *GetOwner();


		/*!
		Set the wait time for the thread termination
		@param[in] milliSec the time for waiting in millisecond
		*/
		virtual void SetWaitTime(unsigned int milliSec);
		
		/*!
		Get the wait time for the parser thread termination
		@return the current time for waiting in millisecond
		*/
		virtual unsigned int GetWaitTime();

		/*!
		Set the Callback Object for the Socket.
		@param[in] callBackObj The Callback Object to set.
		*/
		virtual void SetCallbackObject(ServerCallbackInterface *callBackObj);

		/*!
		Get the Callback Object of the Socket
		@return the current Callback Object
		*/
		virtual ServerCallbackInterface *GetCallbackObject();


	protected:	
	
		/*!
		Actually Kill the connection
		*/
		virtual void killConnection()=0;

		/*!
		thread loop function
		*/
		virtual void execute()=0;

		/*!
		Set the owner for the base server worker thread.
		@param[in] owner The owner of this worker.
		*/
		virtual void setOwner(BaseServerObject * owner );

		/*!
		Set the Sock Address for this socket.
		@param[in] sockAddr The Sock Address for this socket.
		*/
		virtual void setSockAddr(sockaddr sockAddr);

	protected:
		/*!
		Default Copy Constructor

		Initializes the BaseSocket
		@param[in] b the second object
		@remark Copy Constructor prohibited
		*/
		BaseSocket(const BaseSocket& b){}

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		@remark Copy Operator prohibited
		*/
		BaseSocket & operator=(const BaseSocket&b){return *this;}
	
	
	protected:

		/// Owner
		BaseServerObject *m_owner;

		/// general lock 
		epl::BaseLock *m_baseSocketLock;
		
		/// Lock Policy
		epl::LockPolicy m_lockPolicy;

		/// Callback Object
		ServerCallbackInterface *m_callBackObj;

		///Sock Address
		sockaddr m_sockAddr;
	};

}

#endif //__EP_BASE_SOCKET_H__