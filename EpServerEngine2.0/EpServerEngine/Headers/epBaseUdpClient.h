/*! 
@file epBaseUdpClient.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 18, 2012
@brief Base UDP Client Interface
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

An Interface for Base UDP Client.

*/
#ifndef __EP_BASE_CLIENT_UDP_H__
#define __EP_BASE_CLIENT_UDP_H__

#include "epServerEngine.h"
#include "epBaseClient.h"
#include "epServerObjectList.h"
#include "epClientPacketProcessor.h"


namespace epse{

	/*! 
	@class BaseUdpClient epBaseUdpClient.h
	@brief A class for Base UDP Client.
	*/
	class EP_SERVER_ENGINE BaseUdpClient:public BaseClient{

	public:
		/*!
		Default Constructor

		Initializes the Client
		@param[in] callBackObj the call back object
		@param[in] hostName the hostname string
		@param[in] port the port string
		@param[in] waitTimeMilliSec wait time for Client Thread to terminate
		@param[in] lockPolicyType The lock policy
		*/
		BaseUdpClient(ClientCallbackInterface *callBackObj,const TCHAR * hostName=_T(DEFAULT_HOSTNAME), const TCHAR * port=_T(DEFAULT_PORT),unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the BaseClientUDP
		@param[in] b the second object
		*/
		BaseUdpClient(const BaseUdpClient& b);
		/*!
		Default Destructor

		Destroy the Client
		*/
		virtual ~BaseUdpClient();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseUdpClient & operator=(const BaseUdpClient&b);
		

		/*!
		Get the maximum packet byte size
		@return the maximum packet byte size
		*/
		virtual unsigned int GetMaxPacketByteSize() const;

		/*!
		Connect to the server
		@param[in] hostName the hostname string
		@param[in] port the port string
		@remark if argument is NULL then previously setting value is used
		*/
		virtual bool Connect(const TCHAR * hostName=NULL, const TCHAR * port=NULL)=0;

		/*!
		Disconnect from the server
		*/
		virtual void Disconnect()=0;

		/*!
		Send the packet to the server
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@return sent byte size
		@remark return -1 if error occurred
		*/
		int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE);
	
	protected:
		/*!
		Rseet client
		*/
		void resetClient();
	
		
		/*!
		Receive the packet from the server
		@param[out] packet the packet received
		@return received byte size
		*/
		int receive(Packet &packet);

		/*!
		Actually processing the client thread
		@remark  Subclasses must implement this
		*/
		virtual void execute()=0;

		/*!
		Clean up the client initialization.
		*/
		void cleanUpClient();

		/*!
		Actually Disconnect from the server
		*/
		virtual void disconnect()=0;

	protected:
	

		/// Maximum UDP Datagram byte size
		unsigned int m_maxPacketSize;

		/// internal variable2
		struct addrinfo *m_ptr;

	};
}


#endif //__EP_BASE_CLIENT_UDP_H__