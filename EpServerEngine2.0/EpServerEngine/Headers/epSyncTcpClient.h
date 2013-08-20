/*! 
@file epSyncTcpClient.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Synchronous TCP Client Interface
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

An Interface for Synchronous TCP Client.

*/
#ifndef __EP_SYNC_TCP_CLIENT_H__
#define __EP_SYNC_TCP_CLIENT_H__

#include "epServerEngine.h"
#include "epBaseTcpClient.h"

namespace epse{

	/*! 
	@class SyncTcpClient epSyncTcpClient.h
	@brief A class for Synchronous TCP Client.
	*/
	class EP_SERVER_ENGINE SyncTcpClient:public BaseTcpClient{
	public:
		/*!
		Default Constructor

		Initializes the Client
		@param[in] lockPolicyType The lock policy
		*/
		SyncTcpClient(epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);


		/*!
		Default Copy Constructor

		Initializes the Client
		@param[in] b the second object
		*/
		SyncTcpClient(const SyncTcpClient& b);
		/*!
		Default Destructor

		Destroy the Client
		*/
		virtual ~SyncTcpClient();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		SyncTcpClient & operator=(const SyncTcpClient&b);
		
		/*!
		Connect to the server
		@param[in] ops the client options
		@return true if successfully connected otherwise false
		@remark if argument is NULL then previously setting value is used
		*/
		bool Connect(const ClientOps &ops=ClientOps::defaultClientOps);

		/*!
		Disconnect from the server
		*/
		void Disconnect();

		/*!
		Check if the connection is established
		@return true if the connection is established otherwise false
		*/
		bool IsConnectionAlive() const;

		/*!
		Send the packet to the server
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@param[in] sendStatus the status of Send
		@return sent byte size
		@remark return -1 if error occurred
		*/
		int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE,SendStatus *sendStatus=NULL);

		/*!
		Receive the packet from the server
		@param[in] waitTimeInMilliSec wait time for receiving the packet in millisecond
		@param[out] retStatus the status of receive
		@return received packet
		@remark the caller must call ReleaseObj() for Packet to avoid the memory leak.
		*/
		Packet *Receive(unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE,ReceiveStatus *retStatus=NULL);


	private:

		/*!
		Actually processing the client thread
		*/
		virtual void execute();

		/*!
		Actually Disconnect from the server
		*/
		void disconnect();

	private:

		/// Status for connection
		bool m_isConnected;


	};
}


#endif //__EP_SYNC_TCP_CLIENT_H__