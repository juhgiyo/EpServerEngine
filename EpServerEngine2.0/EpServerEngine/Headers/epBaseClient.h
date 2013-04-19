/*! 
@file epBaseClient.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base Client Interface
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

An Interface for Base Client.

*/

#ifndef __EP_BASE_CLIENT_H__
#define __EP_BASE_CLIENT_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include "epServerEngine.h"
#include "epPacket.h"
#include "epBaseServerObject.h"
#include "epServerConf.h"
#include "epClientInterfaces.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


namespace epse{


	/*! 
	@class BaseClient epBaseClient.h
	@brief A class for Base Client.
	*/
	class EP_SERVER_ENGINE BaseClient:public BaseServerObject,public ClientInterface{

	public:
		/*!
		Default Constructor

		Initializes the Client
		@param[in] lockPolicyType The lock policy
		*/
		BaseClient(epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the Client
		@param[in] b the second object
		*/
		BaseClient(const BaseClient& b);
		/*!
		Default Destructor

		Destroy the Client
		*/
		virtual ~BaseClient();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseClient & operator=(const BaseClient&b);
		
		/*!
		Set the hostname for the server.
		@remark Cannot be changed while connected to server
		@param[in] hostName The hostname to set.
		*/
		void SetHostName(const TCHAR * hostName);

		/*!
		Get the hostname of server
		@return the hostname in string
		*/
		epl::EpTString GetHostName() const;

		/*!
		Set the port for the server.
		@remark Cannot be changed while connected to server
		@param[in] port The port to set.
		*/
		void SetPort(const TCHAR *  port);

		/*!
		Get the port number of server
		@return the port number in string
		*/
		epl::EpTString GetPort() const;

		/*!
		Set the Callback Object for the server.
		@param[in] callBackObj The Callback Object to set.
		*/
		void SetCallbackObject(ClientCallbackInterface *callBackObj);

		/*!
		Get the Callback Object of server
		@return the current Callback Object
		*/
		ClientCallbackInterface *GetCallbackObject();

		/*!
		Set the wait time for the thread termination
		@param[in] milliSec the time for waiting in millisecond
		*/
		virtual void SetWaitTime(unsigned int milliSec);
		
		/*!
		Get the wait time for the parser thread termination
		@return the current time for waiting in millisecond
		*/
		unsigned int GetWaitTime() const;

		/*!
		Connect to the server
		@param[in] ops the client options
		@remark if argument is NULL then previously setting value is used
		*/
		virtual bool Connect(const ClientOps &ops=ClientOps::defaultClientOps)=0;

		/*!
		Disconnect from the server
		*/
		virtual void Disconnect()=0;

		/*!
		Check if the connection is alive
		@return true if the connection is alive otherwise false
		*/
		virtual bool IsConnectionAlive() const;

		/*!
		Send the packet to the client
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@return sent byte size
		@remark return -1 if error occurred
		*/
		virtual int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE)=0;
	

	protected:

		/*!
		*/
		SOCKET getSocket();
		SOCKET setSocket(SOCKET sock);

		/*!
		Actually set the hostname for the server.
		@remark Cannot be changed while connected to server
		@param[in] hostName The hostname to set.
		*/
		void setHostName(const TCHAR * hostName);

		/*!
		Actually set the port for the server.
		@remark Cannot be changed while connected to server
		@param[in] port The port to set.
		*/
		void setPort(const TCHAR *port);

		/*!
		Listening Loop Function
		*/
		virtual void execute()=0 ;


		/*!
		Clean up the client initialization.
		*/
		virtual void cleanUpClient();

		/*!
		Reset Client
		*/
		void resetClient();


	protected:
		/// port
		epl::EpString m_port;
		/// hostname
		epl::EpString m_hostName;
		/// internal variable
		struct addrinfo *m_result;

		/// send lock
		epl::BaseLock *m_sendLock;
		/// general lock
		epl::BaseLock *m_generalLock;

		/// Lock Policy
		epl::LockPolicy m_lockPolicy;

		/// Callback Object
		ClientCallbackInterface *m_callBackObj;

	private:
		/// connection socket
		SOCKET m_connectSocket;
	};
}
#endif //__EP_BASE_CLIENT_H__