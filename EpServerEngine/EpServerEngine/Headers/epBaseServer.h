/*! 
@file epBaseServer.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base Server Interface
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

An Interface for Base Server.

*/
#ifndef __EP_BASE_SERVER_H__
#define __EP_BASE_SERVER_H__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include "epServerEngine.h"
#include "epBaseServerWorker.h"
#include "epServerConf.h"
#include "epBaseServerObject.h"
#include "epServerObjectList.h"
#include "epParserList.h"


using namespace std;

namespace epse{
	/*! 
	@class BaseServer epBaseServer.h
	@brief A class for Base Server.
	*/
	class EP_SERVER_ENGINE BaseServer:public BaseServerObject{

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
		BaseServer(const TCHAR * port=_T(DEFAULT_PORT),SyncPolicy syncPolicy=SYNC_POLICY_ASYNCHRONOUS, unsigned int maximumConnectionCount=CONNECTION_LIMIT_INFINITE,unsigned int waitTimeMilliSec=WAITTIME_INIFINITE, epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the BaseServer
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
		@param[in] sockAddrInfo the socket address info
		@remark Sub-class should implement this to create new worker.
		@remark Server will automatically release this worker.
		@remark returning NULL will close the client connection.
		@return the new server worker
		*/
		virtual BaseServerWorker* createNewWorker(sockaddr sockAddrInfo)=0;

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


	private:
		/// port number
		epl::EpString m_port;
		/// listening socket
		SOCKET m_listenSocket;
		/// internal use variable
		struct addrinfo *m_result;
		// Socket info
		sockaddr m_socketInfo;
		// Socket info size
		int m_sockAddrSize;

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
#endif //__EP_BASE_SERVER_H__