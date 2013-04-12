/*! 
@file epBaseServerUDP.h
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
#ifndef __EP_BASE_SERVER_UDP_H__
#define __EP_BASE_SERVER_UDP_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include "epServerEngine.h"
#include "epBaseServerWorkerUDP.h"
#include "epPacket.h"
#include "epServerConf.h"
#include "epBaseServerObject.h"
#include "epServerObjectList.h"
#include "epParserList.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")




using namespace std;

namespace epse{

	/*! 
	@class BaseServerUDP epBaseServerUDP.h
	@brief A class for Base UDP Server.
	*/
	class EP_SERVER_ENGINE BaseServerUDP:public BaseServerObject{
		friend class BaseServerWorkerUDP;
	public:
		
		/*!
		Default Constructor

		Initializes the Server
		@param[in] port the port string
		@param[in] syncPolicy Synchronous Policy
		@param[in] maximumConnectionCount the maximum number of connection
		@param[in] waitTimeMilliSec wait time for Server Thread to terminate
		@param[in] lockPolicyType The lock policy
		*/
		BaseServerUDP(const TCHAR * port=_T(DEFAULT_PORT),SyncPolicy syncPolicy=SYNC_POLICY_ASYNCHRONOUS, unsigned int maximumConnectionCount=CONNECTION_LIMIT_INFINITE,unsigned int waitTimeMilliSec=WAITTIME_INIFINITE, epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the BaseServer
		@param[in] b the second object
		*/
		BaseServerUDP(const BaseServerUDP& b);
		/*!
		Default Destructor

		Destroy the Server
		*/
		virtual ~BaseServerUDP();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseServerUDP & operator=(const BaseServerUDP&b);
		

		/*!
		Get Worker List
		@return the list of the worker
		*/
		vector<BaseServerObject*> GetWorkerList() const;

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
		@return The Maximum Connection Count
		@remark 0 means there is no limit
		*/
		unsigned int GetMaximumConnectionCount() const;
	
		/*!
		Set Synchronous Policy
		@param[in] syncPolicy Synchronous Policy to set
		@return true if successfully set otherwise false
		@remark SyncPolicy cannot be set when Server is running.
		*/
		bool SetSyncPolicy(SyncPolicy syncPolicy);

		/*!
		Get current Synchronous Policy
		@return Synchronous Policy
		*/
		SyncPolicy GetSyncPolicy() const;

		/*!
		Get the maximum packet byte size
		@return the maximum packet byte size
		*/
		unsigned int GetMaxPacketByteSize() const;

		/*!
		Start the server
		@param[in] port the port string
		@remark if argument is NULL then previously setting value is used
		*/
		bool StartServer(const TCHAR * port=NULL);
		/*!
		Stop the server
		*/
		void StopServer();

		/*!
		Check if the server is started
		@return true if the server is started otherwise false
		*/
		bool IsServerStarted() const;

		/*!
		Terminate all clients' socket connected.
		*/
		void ShutdownAllClient();

		/*!
		Broadcast the packet
		@param[in] packet
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond for each connection
		*/
		void Broadcast(const Packet& packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE);

		/*!
		Do the action given by input function for all workers
		@param[in] DoFunc the action for each worker
		@param[in] argCount the number of arguments
		*/
		void CommandWorkers(void (__cdecl *DoFunc)(BaseServerObject*,unsigned int,va_list),unsigned int argCount,...);

		/*!
		Find with given key by comparing worker with given function
		@param[in] key the key to find
		@param[in] EqualFunc the Compare Function
		@return the found BaseServerObject
		*/
		template <typename T>
		BaseServerObject  *FindWorker(T const & key, bool (__cdecl *EqualFunc)(T const &, const BaseServerObject *))
		{
			return m_workerList.Find(key,EqualFunc);
		}

	protected:
		/*!
		Return the new server worker.
		@remark Sub-class should implement this to create new worker.
		@remark Server will automatically release this worker.
		@return the new server worker
		*/
		virtual BaseServerWorkerUDP* createNewWorker()=0;

	private:
		/*!
		Actually set the port for the server.
		@remark Cannot be changed while connected to server
		@param[in] port The port to set.
		*/
		void setPort(const TCHAR *port);

		/*!
		Listening Loop Function
		*/
		virtual void execute() ;
		
		/*!
		Kill connection from the client
		@param[in] clientObj client object
		@param[in] argCount the argument count
		@param[in] args the argument list
		*/
		static void killConnection(BaseServerObject *clientObj,unsigned int argCount,va_list args);

		/*!
		Send packet to given client
		@param[in] clientObj client object
		@param[in] argCount the argument count
		@param[in] args the argument list
		*/
		static void sendPacket(BaseServerObject *clientObj,unsigned int argCount,va_list args);

		/*!
		Send the packet to the client
		@param[in] packet the packet to be sent
		@param[in] clientSockAddr the client socket address, which the packet will be delivered
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@return sent byte size
		@remark return -1 if error occurred
		*/
		int send(const Packet &packet,const sockaddr &clientSockAddr, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE);

		/*!
		Clean up the server initialization.
		*/
		void cleanUpServer();

		/*!
		Actually shut down all the clients (worker).
		*/
		void shutdownAllClient();

		/*!
		Actually Stop the server
		*/
		void stopServer();

		/*!
		Reset Server
		*/
		void resetServer();
		/*!
		Compare given clientSocket with BaseServerObject's socket
		@param[in] clientSocket socket to compare
		@param[in] obj the BaseServerObject pointer
		@return true if same socket otherwise false
		*/
		static bool socketCompare(sockaddr const & clientSocket, const BaseServerObject*obj );



	private:
		/// port number
		epl::EpString m_port;
		/// listening socket
		SOCKET m_listenSocket;
		/// internal use variable
		struct addrinfo *m_result;

		/// Maximum UDP Datagram byte size
		unsigned int m_maxPacketSize;

		/// send lock
		epl::BaseLock *m_sendLock;
		/// general lock 
		epl::BaseLock *m_baseServerLock;

		/// Lock Policy
		epl::LockPolicy m_lockPolicy;

		/// worker list
		ServerObjectList m_workerList;

		/// Server Parser List
		ParserList *m_parserList;

		/// Maximum Connection Count
		unsigned int m_maxConnectionCount;

	};
}
#endif //__EP_BASE_SERVER_UDP_H__