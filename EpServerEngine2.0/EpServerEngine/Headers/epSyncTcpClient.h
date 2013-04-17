/*! 
@file epSyncTcpClient.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Synchronous TCP Client Interface
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
		@param[in] callBackObj the call back object
		@param[in] hostName the hostname string
		@param[in] port the port string
		@param[in] lockPolicyType The lock policy
		*/
		SyncTcpClient(ClientCallbackInterface *callBackObj,const TCHAR * hostName=_T(DEFAULT_HOSTNAME), const TCHAR * port=_T(DEFAULT_PORT),epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Constructor

		Initializes the Client
		@param[in] ops the client options
		*/
		SyncTcpClient(const ClientOps &ops);

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
		@param[in] hostName the hostname string
		@param[in] port the port string
		@remark if argument is NULL then previously setting value is used
		*/
		bool Connect(const TCHAR * hostName=NULL, const TCHAR * port=NULL);

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