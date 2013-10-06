/*! 
@file epBaseUdpSocket.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base UDP Socket Interface
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

An Interface for Base UDP Socket.

*/
#ifndef __EP_BASE_UDP_SOCKET_H__
#define __EP_BASE_UDP_SOCKET_H__

#include "epServerEngine.h"
#include "epBaseSocket.h"
#include <queue>
using namespace std;

namespace epse
{
	class SyncUdpServer;
	/*! 
	@class BaseUdpSocket epBaseUdpSocket.h
	@brief A class for Base UDP Socket.
	*/
	class EP_SERVER_ENGINE BaseUdpSocket:public BaseSocket
	{
		friend class SyncUdpServer;
		friend class AsyncUdpServer;
	public:
		/*!
		Default Constructor

		Initializes the Socket
		@param[in] callBackObj the callback object
		@param[in] waitTimeMilliSec wait time for Socket Thread to terminate
		@param[in] lockPolicyType The lock policy
		*/
		BaseUdpSocket(ServerCallbackInterface *callBackObj,unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Destructor

		Destroy the Socket
		*/
		virtual ~BaseUdpSocket();

		
		/*!
		Send the packet to the client
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@param[in] sendStatus the status of Send
		@return sent byte size
		@remark return -1 if error occurred
		*/
		int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE,SendStatus *sendStatus=NULL);

		
		/*!
		Kill the connection
		*/
		void KillConnection()=0;

	
		/*!
		Get the maximum packet byte size
		@return the maximum packet byte size
		*/
		unsigned int GetMaxPacketByteSize() const;


	protected:	
		friend class BaseServerUDP;

		/*!
		Actually Kill the connection
		*/
		virtual void killConnection()=0;

		
		/*!
		thread loop function
		*/
		virtual void execute()=0;
		
		/*!
		Add new packet received from client
		@param[in] packet the new packet received from client
		*/
		virtual void addPacket(Packet *packet)=0;
	
		/*!
		Set the maximum packet byte size
		@param[in] maxPacketSize the maximum packet byte size to set
		*/
		void setMaxPacketByteSize(unsigned int maxPacketSize);
	protected:
		/*!
		Default Copy Constructor

		Initializes the BaseUdpSocket
		@param[in] b the second object
		*/
		BaseUdpSocket(const BaseUdpSocket& b):BaseSocket(b)
		{}


		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseUdpSocket & operator=(const BaseUdpSocket&b){return *this;}
	protected:

		/// Maximum UDP Datagram byte size
		unsigned int m_maxPacketSize;

		/// list lock 
		epl::BaseLock *m_listLock;

		/// Packet List
		queue<Packet*> m_packetList;
	};

}

#endif //__EP_BASE_UDP_SOCKET_H__