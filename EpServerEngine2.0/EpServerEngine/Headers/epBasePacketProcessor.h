/*! 
@file epBasePacketProcessor.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 20, 2012
@brief Base Packet Processor Interface
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

An Interface for Base Packet Processor.

*/
#ifndef __EP_BASE_PACKET_PROCESSOR_H__
#define __EP_BASE_PACKET_PROCESSOR_H__

#include "epServerEngine.h"
#include "epPacket.h"
#include "epBaseServerObject.h"
#include "epServerConf.h"

namespace epse
{
	/*! 
	@class BasePacketProcessor epBasePacketProcessor.h
	@brief A class for Base Packet Processor.
	*/
	class EP_SERVER_ENGINE BasePacketProcessor:public BaseServerObject
	{
	public:
		/*!
		Default Constructor

		Initializes the Processor
		@param[in] waitTimeMilliSec the wait time in millisecond for terminating
		@param[in] lockPolicyType The lock policy
		*/
		BasePacketProcessor(unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Destructor

		Destroy the Processor
		*/
		virtual ~BasePacketProcessor();

	protected:
		/*!
		Default Copy Constructor

		Initializes the BasePacketProcessor
		@param[in] b the second object
		@remark Copy Constructor prohibited
		*/
		BasePacketProcessor(const BasePacketProcessor& b){}

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		@remark Copy Operator prohibited
		*/
		BasePacketProcessor & operator=(const BasePacketProcessor&b){return *this;}


	protected:	
		friend class AsyncTcpClient;
		friend class AsyncUdpClient;

		friend class BaseSocket;
		friend class AsyncTcpSocket;
		friend class AsyncUdpSocket;

		/*! 
		@struct PacketPassUnit epBasePacketProcessor.h
		@brief A class for Packet Passing Unit for Packet Processing Thread.
		*/
		struct PacketPassUnit{
			/// BaseServerWorkerEx Object
			BaseServerObject *m_owner;
			/// Packet to parse
			Packet *m_packet;
		};

		/*!
		Set PacketPassUnit
		@param[in] packetPassUnit PacketPassUnit to set
		*/
		void setPacketPassUnit(const PacketPassUnit& packetPassUnit);	
		

		/*!
		thread loop function
		*/
		virtual void execute()=0;


	protected:

        /// Owner
		BaseServerObject *m_owner;

		/// Packet received
		Packet * m_packetReceived;


	};

}

#endif //__EP_BASE_PACKET_PROCESSOR_H__