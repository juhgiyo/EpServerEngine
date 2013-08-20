/*! 
@file epBaseTcpClient.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base TCP Client Interface
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

An Interface for Base TCP Client.

*/
#ifndef __EP_BASE_TCP_CLIENT_H__
#define __EP_BASE_TCP_CLIENT_H__

#include "epServerEngine.h"
#include "epBaseClient.h"

namespace epse{

	/*! 
	@class BaseTcpClient epBaseTcpClient.h
	@brief A class for Base TCP Client.
	*/
	class EP_SERVER_ENGINE BaseTcpClient:public BaseClient{
	public:
		/*!
		Default Constructor

		Initializes the Client
		@param[in] lockPolicyType The lock policy
		*/
		BaseTcpClient(epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);


		/*!
		Default Copy Constructor

		Initializes the Client
		@param[in] b the second object
		*/
		BaseTcpClient(const BaseTcpClient& b);
		/*!
		Default Destructor

		Destroy the Client
		*/
		virtual ~BaseTcpClient();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseTcpClient & operator=(const BaseTcpClient&b);
	
		/*!
		Connect to the server
		@param[in] ops the client options
		@return true if successfully connected otherwise false
		@remark if argument is NULL then previously setting value is used
		*/
		virtual bool Connect(const ClientOps &ops=ClientOps::defaultClientOps)=0;

		/*!
		Disconnect from the server
		*/
		virtual void Disconnect()=0;


		/*!
		Send the packet to the server
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@param[in] sendStatus the status of Send
		@return sent byte size
		@remark return -1 if error occurred
		*/
		virtual int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE,SendStatus *sendStatus=NULL);

	protected:

	

		/*!
		Receive the packet from the server
		@param[out] packet the packet received
		@return received byte size
		*/
		int receive(Packet &packet);

		/*!
		Actually processing the client thread
		*/
		virtual void execute()=0;

		/*!
		Actually Disconnect from the server
		*/
		virtual void disconnect()=0;


	protected:

		/// Temp Packet;
		Packet m_recvSizePacket;


	};
}


#endif //__EP_BASE_TCP_CLIENT_H__