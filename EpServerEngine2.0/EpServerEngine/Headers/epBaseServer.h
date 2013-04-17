/*! 
@file epBaseServer.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base Server Interface
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

An Interface for Base Server.

*/
#ifndef __EP_BASE_SERVER_H__
#define __EP_BASE_SERVER_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN


#include "epServerEngine.h"
#include "epServerConf.h"
#include "epBaseServerObject.h"
#include "epServerInterfaces.h"
#include "epServerObjectList.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")


namespace epse{

	

	/*! 
	@class BaseServer epBaseServer.h
	@brief A class for Base Server.
	*/
	class EP_SERVER_ENGINE BaseServer:public BaseServerObject,public ServerInterface{

	public:
		/*!
		Default Constructor

		Initializes the Server
		@param[in] callBackObj the callback object
		@param[in] port the port string
		@param[in] maximumConnectionCount the maximum number of connection
		@param[in] waitTimeMilliSec wait time for Server Thread to terminate
		@param[in] lockPolicyType The lock policy
		*/
		BaseServer(ServerCallbackInterface *callBackObj,const TCHAR * port=_T(DEFAULT_PORT),unsigned int waitTimeMilliSec=WAITTIME_INIFINITE, unsigned int maximumConnectionCount=CONNECTION_LIMIT_INFINITE, epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Constructor

		Initializes the Server
		@param[in] ops the server options
		*/
		BaseServer(const ServerOps &ops);

		/*!
		Default Copy Constructor

		Initializes the Server
		@param[in] b the second object
		*/
		BaseServer(const BaseServer& b);
		/*!
		Default Destructor

		Destroy the Server
		*/
		virtual ~BaseServer();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseServer & operator=(const BaseServer&b);
		

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
		Set the Maximum Connection Count for the server.
		@param[in] maxConnectionCount The Maximum Connection Count to set.
		@remark 0 means there is no limit
		*/
		void SetMaximumConnectionCount(unsigned int maxConnectionCount);

		/*!
		Get the Maximum Connection Count of server
		@return the Maximum Connection Count
		@remark 0 means there is no limit
		*/
		unsigned int GetMaximumConnectionCount() const;

		/*!
		Set the Callback Object for the server.
		@param[in] callBackObj The Callback Object to set.
		*/
		void SetCallbackObject(ServerCallbackInterface *callBackObj);
		
		/*!
		Get the Callback Object of server
		@return the current Callback Object
		*/
		ServerCallbackInterface *GetCallbackObject();

		/*!
		Set the wait time for the thread termination
		@param[in] milliSec the time for waiting in millisecond
		*/
		void SetWaitTime(unsigned int milliSec);


		/*!
		Start the server
		@param[in] port the port string
		@remark if argument is NULL then previously setting value is used
		*/
		virtual bool StartServer(const TCHAR * port=NULL)=0;
		/*!
		Stop the server
		*/
		virtual void StopServer()=0;

		/*!
		Check if the server is started
		@return true if the server is started otherwise false
		*/
		virtual bool IsServerStarted() const;

		/*!
		Terminate all clients' socket connected.
		*/
		void ShutdownAllClient();

	protected:
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
		Clean up the server initialization.
		*/
		virtual void cleanUpServer();

		/*!
		Reset Server
		*/
		virtual void resetServer();

		/*!
		Kill connection from the client
		@param[in] clientObj client object
		@param[in] argCount the argument count
		@param[in] args the argument list
		*/
		static void killConnection(BaseServerObject *clientObj,unsigned int argCount,va_list args);

		/*!
		Actually shut down all the clients (sockets).
		*/
		void shutdownAllClient();

		/*!
		Actually Stop the server
		*/
		void stopServer();



	protected:
		/// port number
		epl::EpString m_port;
		/// listening socket
		SOCKET m_listenSocket;
		/// internal use variable
		struct addrinfo *m_result;

		/// general lock 
		epl::BaseLock *m_baseServerLock;

		/// Lock Policy
		epl::LockPolicy m_lockPolicy;

		/// worker list
		ServerObjectList m_socketList;

		/// Maximum Connection Count
		unsigned int m_maxConnectionCount;
	
		/// Callback Object
		ServerCallbackInterface *m_callBackObj;
	};
}
#endif //__EP_BASE_SERVER_H__