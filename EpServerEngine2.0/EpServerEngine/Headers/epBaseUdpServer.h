/*! 
@file epBaseUdpServer.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base UDP Server Interface
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

An Interface for Base UDP Server.

*/
#ifndef __EP_BASE_UDP_SERVER_H__
#define __EP_BASE_UDP_SERVER_H__


#include "epServerEngine.h"
#include "epBaseServer.h"

namespace epse{

	/*! 
	@class BaseUdpServer epBaseUdpServer.h
	@brief A class for Base UDP Server.
	*/
	class EP_SERVER_ENGINE BaseUdpServer:public BaseServer
	{
		friend class AsyncUdpSocket;
		friend class SyncUdpSocket;
		friend class BaseUdpSocket;
	public:
		
		/*!
		Default Constructor

		Initializes the Server
		@param[in] lockPolicyType The lock policy
		*/
		BaseUdpServer(epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);


		/*!
		Default Copy Constructor

		Initializes the Server
		@param[in] b the second object
		*/
		BaseUdpServer(const BaseUdpServer& b);
		/*!
		Default Destructor

		Destroy the Server
		*/
		virtual ~BaseUdpServer();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseUdpServer & operator=(const BaseUdpServer&b);
		
		/*!
		Start the server
		@param[in] ops the server options
		@return true if successfully started otherwise false
		@remark if argument is NULL then previously setting value is used
		*/
		virtual bool StartServer(const ServerOps &ops=ServerOps::defaultServerOps);
		/*!
		Stop the server
		*/
		void StopServer();

		/*!
		Get the maximum packet byte size
		@return the maximum packet byte size
		*/
		unsigned int GetMaxPacketByteSize() const;

	protected:
	
		/*!
		Listening Loop Function
		*/
		virtual void execute()=0;
	
		/*!
		Clean up the server initialization.
		*/
		virtual void cleanUpServer();

		/*!
		Reset Server
		*/
		virtual void resetServer();



		/*!
		Send the packet to the client
		@param[in] packet the packet to be sent
		@param[in] clientSockAddr the client socket address, which the packet will be delivered
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@param[in] sendStatus the status of Send
		@return sent byte size
		@remark return -1 if error occurred
		*/
		int send(const Packet &packet,const sockaddr &clientSockAddr, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE,SendStatus *sendStatus=NULL);


		/*!
		Compare given clientSocket with BaseServerObject's socket
		@param[in] clientSocket socket to compare
		@param[in] obj the BaseServerObject pointer
		@return true if same socket otherwise false
		*/
		static bool socketCompare(sockaddr const & clientSocket, const BaseServerObject*obj );



	protected:
		/// Maximum UDP Datagram byte size
		unsigned int m_maxPacketSize;

		/// send lock
		epl::BaseLock *m_sendLock;

	};
}
#endif //__EP_BASE_UDP_SERVER_H__