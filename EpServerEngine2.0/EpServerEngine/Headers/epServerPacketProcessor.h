/*! 
@file epServerPacketProcessor.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 20, 2012
@brief Server Packet Processor Interface
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

An Interface for Server Packet Processor.

*/
#ifndef __EP_SERVER_PACKET_PROCESSOR_H__
#define __EP_SERVER_PACKET_PROCESSOR_H__

#include "epServerEngine.h"
#include "epServerInterfaces.h"
#include "epBasePacketProcessor.h"

namespace epse
{
	/*! 
	@class ServerPacketProcessor epServerPacketProcessor.h
	@brief A class for Server Packet Processor.
	*/
	class EP_SERVER_ENGINE ServerPacketProcessor:public BasePacketProcessor
	{
	public:
		/*!
		Default Constructor

		Initializes the Processor
		@param[in] callBackObj the callback object
		@param[in] waitTimeMilliSec the wait time in millisecond for terminating
		@param[in] lockPolicyType The lock policy
		*/
		ServerPacketProcessor(ServerCallbackInterface *callBackObj,unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Destructor

		Destroy the Processor
		*/
		virtual ~ServerPacketProcessor();

	private:
		/*!
		Default Copy Constructor

		Initializes the ServerPacketProcessor
		@param[in] b the second object
		@remark Copy Constructor prohibited
		*/
		ServerPacketProcessor(const ServerPacketProcessor& b):BasePacketProcessor(b)
		{}

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		@remark Copy Operator prohibited
		*/
		ServerPacketProcessor & operator=(const ServerPacketProcessor&b){return *this;}


	private:	
		friend class BaseSocket;
		friend class AsyncTcpSocket;
		friend class AsyncUdpSocket;

	
		/*!
		thread loop function
		*/
		virtual void execute();


	private:
    
		/// Callback Object
		ServerCallbackInterface *m_callBackObj;


	};

}

#endif //__EP_SERVER_PACKET_PROCESSOR_H__