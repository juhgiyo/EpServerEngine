/*! 
@file epBaseUdpServer.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base UDP Server Interface
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