/*! 
@file epClientPacketProcessor.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 20, 2012
@brief Client Packet Processor Interface
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

An Interface for Client Packet Processor.

*/
#ifndef __EP_CLIENT_PACKET_PROCESSOR_H__
#define __EP_CLIENT_PACKET_PROCESSOR_H__

#include "epServerEngine.h"
#include "epClientInterfaces.h"
#include "epBasePacketProcessor.h"
namespace epse
{
	/*! 
	@class ClientPacketProcessor epClientPacketProcessor.h
	@brief A class for Client Packet Processor.
	*/
	class EP_SERVER_ENGINE ClientPacketProcessor:public BasePacketProcessor
	{
	public:
		/*!
		Default Constructor

		Initializes the Processor
		@param[in] callBackObj the callback object
		@param[in] waitTimeMilliSec the wait time in millisecond for terminating
		@param[in] lockPolicyType The lock policy
		*/
		ClientPacketProcessor(ClientCallbackInterface *callBackObj,unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Destructor

		Destroy the Processor
		*/
		virtual ~ClientPacketProcessor();

	private:
		/*!
		Default Copy Constructor

		Initializes the ClientPacketProcessor
		@param[in] b the second object
		@remark Copy Constructor prohibited
		*/
		ClientPacketProcessor(const ClientPacketProcessor& b):BasePacketProcessor(b)
		{}

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		@remark Copy Operator prohibited
		*/
		ClientPacketProcessor & operator=(const ClientPacketProcessor&b){return *this;}


	private:	
		friend class AsyncTcpClient;
	
	
		/*!
		thread loop function
		*/
		virtual void execute();


	private:

		/// Callback Object
		ClientCallbackInterface *m_callBackObj;


	};

}

#endif //__EP_CLIENT_PACKET_PROCESSOR_H__