/*! 
@file epBaseClientManual.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base Client Manual Interface
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

An Interface for Base Client Manual.

*/
#ifndef __EP_BASE_CLIENT_MANUAL_H__
#define __EP_BASE_CLIENT_MANUAL_H__

#include "epServerEngine.h"
#include "epPacket.h"
#include "epBaseServerSendObject.h"
#include "epServerConf.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

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



using namespace std;


namespace epse{

	/*! 
	@class BaseClientManual epBaseClientManual.h
	@brief A class for Base Client Manual.
	*/
	class EP_SERVER_ENGINE BaseClientManual:public BaseServerSendObject{
	public:
		/*!
		Default Constructor

		Initializes the Client
		@param[in] hostName the hostname string
		@param[in] port the port string
		@param[in] lockPolicyType The lock policy
		*/
		BaseClientManual(const TCHAR * hostName=_T(DEFAULT_HOSTNAME), const TCHAR * port=_T(DEFAULT_PORT),epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the BaseClientManual
		@param[in] b the second object
		*/
		BaseClientManual(const BaseClientManual& b);
		/*!
		Default Destructor

		Destroy the Client
		*/
		virtual ~BaseClientManual();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseClientManual & operator=(const BaseClientManual&b);
		

		/*!
		Set the hostname for the server.
		@remark Cannot be changed while connected to server
		@param[in] hostName The hostname to set.
		*/
		void SetHostName(const TCHAR * hostName);

		/*!
		Set the port for the server.
		@remark Cannot be changed while connected to server
		@param[in] port The port to set.
		*/
		void SetPort(const TCHAR * port);

		/*!
		Get the hostname of server
		@return the hostname in string
		*/
		epl::EpTString GetHostName() const;

		/*!
		Get the port number of server
		@return the port number in string
		*/
		epl::EpTString GetPort() const;

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
		bool IsConnected() const;

		/*!
		Send the packet to the server
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@return sent byte size
		@remark return -1 if error occurred
		*/
		virtual int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE);

		/*!
		Receive the packet from the server
		@param[in] waitTimeInMilliSec wait time for receiving the packet in millisecond
		@return received packet
		@remark the caller must call ReleaseObj() for Packet to avoid the memory leak.
		@remark if ClientSynchronousPolicy!=CLIENT_SYNC_POLICY_MANUAL then assertion.
		*/
		Packet *Receive(unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE);


	private:

		/*!
		Reset client
		*/
		void resetClient();
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
		Receive the packet from the server
		@param[out] packet the packet received
		@return received byte size
		*/
		int receive(Packet &packet);


		/*!
		Clean up the client initialization.
		*/
		void cleanUpClient();

		/*!
		Actually Disconnect from the server
		*/
		void disconnect();

	private:
		/// port
		epl::EpString m_port;
		/// hostname
		epl::EpString m_hostName;
		/// connection socket
		SOCKET m_connectSocket;
		/// internal variable
		struct addrinfo *m_result;
		
	
		/// send lock
		epl::BaseLock *m_sendLock;
		/// general lock
		epl::BaseLock *m_generalLock;

		/// Lock Policy
		epl::LockPolicy m_lockPolicy;

		/// Temp Packet;
		Packet m_recvSizePacket;

		/// Status for connection
		bool m_isConnected;


	};
}


#endif //__EP_BASE_CLIENT_MANUAL_H__