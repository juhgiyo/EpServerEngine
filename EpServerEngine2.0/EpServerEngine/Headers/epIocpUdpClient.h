/*! 
@file epIocpUdpClient.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 18, 2012
@brief IOCP UDP Client Interface
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

An Interface for IOCP UDP Client.

*/
#ifndef __EP_IOCP_UDP_CLIENT_H__
#define __EP_IOCP_UDP_CLIENT_H__

#include "epServerEngine.h"
#include "epBaseUdpClient.h"

#include <vector>
#include <queue>

using namespace std;

namespace epse{

	/*! 
	@class IocpUdpClient epIocpUdpClient.h
	@brief A class for IOCP UDP Client.
	*/
	class EP_SERVER_ENGINE IocpUdpClient:public BaseUdpClient, public WorkerThreadDelegate{

	public:
		/*!
		Default Constructor

		Initializes the Client
		@param[in] lockPolicyType The lock policy
		*/
		IocpUdpClient(epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);


		/*!
		Default Copy Constructor

		Initializes the Client
		@param[in] b the second object
		*/
		IocpUdpClient(const IocpUdpClient& b);
		/*!
		Default Destructor

		Destroy the Client
		*/
		virtual ~IocpUdpClient();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		IocpUdpClient & operator=(const IocpUdpClient&b);


	
		/*!
		Connect to the server
		@param[in] ops the client options
		@return true if successfully connected otherwise false
		@remark if argument is NULL then previously setting value is used
		*/
		bool Connect(const ClientOps &ops=ClientOps::defaultClientOps);

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
		Send the packet to the server
		@param[in] packet the packet to be sent
		@param[in] complentionEvent the event to set when send is completed
		@param[in] callBackObj the object to callback when send is completed
		@param[in] priority the priority of the send job
		@remark if the completionEvent and callBackObj are set as NULL, it is ignored.
		*/
		void Send(Packet &packet,EventEx *completionEvent=NULL,ClientCallbackInterface *callBackObj=NULL,Priority priority=PRIORITY_NORMAL);


		/*!
		Receive the packet from the client
		@param[in] complentionEvent the event to set when receive is completed
		@param[in] callBackObj the object to callback when receive is completed
		@param[in] priority the priority of the receive job
		@remark if the completionEvent and callBackObj are set as NULL, it is ignored.
		*/
		void Receive(EventEx *completionEvent=NULL,ClientCallbackInterface *callBackObj=NULL,Priority priority=PRIORITY_NORMAL);

	private:

		/*!
		Send the packet to the server
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@return sent byte size
		@remark return -1 if error occurred
		*/
		int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE,SendStatus *sendStatus=NULL);
	
		/*!
		Receive the packet from the server
		@param[in] waitTimeInMilliSec wait time for receiving the packet in millisecond
		@param[out] retStatus the status of receive
		@return received packet
		@remark the caller must call ReleaseObj() for Packet to avoid the memory leak.
		*/
		Packet *Receive(unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE,ReceiveStatus *retStatus=NULL);
	

		/*!
		Actually processing the client thread
		*/
		virtual void execute();

		/*!
		Actually Disconnect from the server
		*/
		void disconnect();

	private:
		/*!
		Call Back Function.
		@param[in] p the argument for call back function.
		*/
		virtual void CallBackFunc(BaseWorkerThread *p);

		/*!
		Add new job to the worker thread.
		@param[in] job the job to push to the worker thread.
		*/
		void pushJob(BaseJob * job);

		/// Flag for connection
		bool m_isConnected;

		/// general lock 
		epl::BaseLock *m_workerLock;

		/// Worker thread list
		vector<BaseWorkerThread*> m_workerList;
		/// worker thread list with no job
		queue<BaseWorkerThread*> m_emptyWorkerList;
	};
}


#endif //__EP_IOCP_UDP_CLIENT_H__