/*! 
@file epClientInterfaces.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Client Interfaces
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

An Interface for Client Interfaces.

*/
#ifndef __EP_CLIENT_INTERFACES_H__
#define __EP_CLIENT_INTERFACES_H__

#include "epServerEngine.h"
#include <winsock2.h>
#include "epPacket.h"
#include "epBaseServerObject.h"

namespace epse{
	class ClientCallbackInterface;

	/*! 
	@class ClientInterface epClientInterfaces.h
	@brief A class for Client Interface.
	*/
	class EP_SERVER_ENGINE ClientInterface{
	public:

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
		Set the Maximum Processor Count for the Socket.
		@param[in] maxProcessorCount The Maximum Processor Count to set.
		@remark 0 means there is no limit
		@remark For Asynchronous Socket Use Only!
		*/
		virtual void SetMaximumProcessorCount(unsigned int maxProcessorCount){}

		/*!
		Get the Maximum Processor Count of the Socket
		@return The Maximum Processor Count
		@remark 0 means there is no limit
		@remark For Asynchronous Socket Use Only!
		*/
		virtual unsigned int GetMaximumProcessorCount() const{return 0;}

		/*!
		Set the wait time for the thread termination
		@param[in] milliSec the time for waiting in millisecond
		*/
		virtual void SetWaitTime(unsigned int milliSec)=0;
		
		/*!
		Get the wait time for the parser thread termination
		@return the current time for waiting in millisecond
		*/
		virtual unsigned int GetWaitTime()=0;

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
		Check if the connection is alive
		@return true if the connection is alive otherwise false
		*/
		virtual bool IsConnectionAlive() const=0;

		/*!
		Send the packet to the client
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@return sent byte size
		@remark return -1 if error occurred
		*/
		virtual int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE)=0;

		/*!
		Receive the packet from the client
		@param[in] waitTimeInMilliSec wait time for receiving the packet in millisecond
		@param[out] retStatus the pointer to ReceiveStatus enumerator to get receive status.
		@return received packet
		@remark the caller must call ReleaseObj() for Packet to avoid the memory leak.
		@remark For Synchronous Socket Use Only!
		*/
		virtual Packet *Receive(unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE,ReceiveStatus *retStatus=NULL)
		{
			if(retStatus)
				*retStatus=RECEIVE_STATUS_FAIL_NOT_SUPPORTED;
			return NULL;
		}
	
		/*!
		Set the Callback Object for the server.
		@param[in] callBackObj The Callback Object to set.
		*/
		virtual void SetCallbackObject(ClientCallbackInterface *callBackObj)=0;

		/*!
		Get the Callback Object of server
		@return the current Callback Object
		*/
		virtual ClientCallbackInterface *GetCallbackObject()=0;

	
		/*!
		Get the asynchronous receive flag for the Socket.
		@return The flag whether to receive asynchronously.
		@remark for Asynchronous Socket Use Only!
		*/
		virtual bool GetIsAsynchronousReceive() const
		{
			return false;
		}

		/*!
		Set the asynchronous receive flag for the Socket.
		@param[in] isASynchronousReceive The flag whether to receive asynchronously.
		@remark for Asynchronous Socket Use Only!
		*/
		virtual void SetIsAsynchronousReceive(bool isASynchronousReceive)
		{
			return;
		}

		/*!
		Get the maximum packet byte size
		@return the maximum packet byte size
		@remark for UDP Socket Use Only!
		*/
		virtual unsigned int GetMaxPacketByteSize() const{return 0;}

		

	};

	/*! 
	@class ClientCallbackInterface epClientInterfaces.h
	@brief A class for Client Callback Interface.
	*/
	class EP_SERVER_ENGINE ClientCallbackInterface{
	public:
		virtual void OnReceived(ClientInterface *client,const Packet&recievedPacket)=0;
		virtual void OnDisconnect(ClientInterface *client){}
	};
}

#endif //__EP_CLIENT_INTERFACES_H__