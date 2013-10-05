/*! 
@file epBaseServerWorker.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base Worker Interface
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

An Interface for Base Server Worker.

*/
#ifndef __EP_BASE_SERVER_WORKER_H__
#define __EP_BASE_SERVER_WORKER_H__

#include "epServerEngine.h"
#include "epPacket.h"
#include "epBaseServerSendObject.h"
#include "epBasePacketParser.h"
#include "epServerConf.h"
#include "epParserList.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <vector>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

using namespace std;

namespace epse
{
	class BaseServer;

	/*! 
	@class BaseServerWorker epBaseServerWorker.h
	@brief A class for Base Server Worker.
	*/
	class EP_SERVER_ENGINE BaseServerWorker:public BaseServerSendObject
	{
	public:
		/*!
		Default Constructor

		Initializes the Worker
		@param[in] maximumParserCount the maximum number of parser
		@param[in] waitTimeMilliSec wait time for Worker Thread to terminate
		@param[in] lockPolicyType The lock policy
		*/
		BaseServerWorker(unsigned int maximumParserCount=PARSER_LIMIT_INFINITE,unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the BaseServerWorker
		@param[in] b the second object
		*/
		BaseServerWorker(const BaseServerWorker& b);

		/*!
		Default Destructor

		Destroy the Worker
		*/
		virtual ~BaseServerWorker();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseServerWorker & operator=(const BaseServerWorker&b);
	

		/*!
		Send the packet to the client
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@return sent byte size
		@remark return -1 if error occurred
		*/
		virtual int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE);

		/*!
		Set the Maximum Parser Count for the server.
		@param[in] maxParserCount The Maximum Parser Count to set.
		@remark 0 means there is no limit
		*/
		void SetMaximumParserCount(unsigned int maxParserCount);

		/*!
		Get the Maximum Parser Parser of server
		@return The Maximum Connection Count
		@remark 0 means there is no limit
		*/
		unsigned int GetMaximumParserCount() const;

		/*!
		Get Packet Parser List
		@return the list of the packet parser
		*/
		vector<BaseServerObject*> GetPacketParserList() const;

		/*!
		Check if the connection is alive
		@return true if the connection is alive otherwise false
		*/
		bool IsConnectionAlive() const;

		/*!
		Kill the connection
		*/
		void KillConnection();

		/*!
		Get the owner object of this worker object.
		@return the pointer to the owner object.
		*/
		BaseServer *GetOwner() const;

		/*!
		Get the IP of client
		@return the IP of client in string
		*/
		epl::EpTString GetIP() const;

		/*!
		Get the sockaddr of client
		@return the sockaddr of client
		*/
		sockaddr GetSockAddr() const;

	protected:
		/*!
		Return the new packet parser
		@remark Sub-class should implement this to create new parser.
		@remark Client will automatically release this parser.
		@remark returning NULL will ignore the packet.
		@return the new packet parser
		*/
		virtual BasePacketParser* createNewPacketParser()=0;



	private:	
		friend class BaseServer;
		/*!
		Set Parser List for current worker
		@param[in] parserList the parser list to set
		*/
		void setParserList(ParserList *parserList);
	
		/*!
		Reset worker
		*/
		void resetWorker();
		/*!
		Actually Kill the connection
		*/
		void killConnection();

		/*!
		thread loop function
		*/
		virtual void execute();

		/*!
		Receive the packet from the client
		@remark  Subclasses must implement this
		@param[out] packet the packet received
		@return received byte size
		*/
		int receive(Packet &packet);
	
		/*!
		Set the argument for the base server worker thread.
		@param[in] clientSocket The client socket from server.
		*/
		void setClientSocket(const SOCKET& clientSocket );

		/*!
		Set the owner for the base server worker thread.
		@param[in] owner The owner of this worker.
		*/
		void setOwner(BaseServer * owner );
	
	private:

	
		/// client socket
		SOCKET m_clientSocket;

		/// Owner
		BaseServer *m_owner;

		/// send lock
		epl::BaseLock *m_sendLock;

		/// general lock 
		epl::BaseLock *m_baseWorkerLock;
		
		/// Lock Policy
		epl::LockPolicy m_lockPolicy;

		/// Temp Packet;
		Packet m_recvSizePacket;

		/// parser thread list
		ParserList *m_parserList;

		/// Maximum Parser Count
		unsigned int m_maxParserCount;
	};

}

#endif //__EP_BASE_SERVER_WORKER_H__