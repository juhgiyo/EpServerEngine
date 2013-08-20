/*! 
@file epBasePacketParser.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 20, 2012
@brief Base Packet Parser Interface
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

An Interface for Base Packet Parser.

*/
#ifndef __EP_BASE_PACKET_PARSER_H__
#define __EP_BASE_PACKET_PARSER_H__

#include "epServerEngine.h"
#include "epPacket.h"
#include "epBaseServerSendObject.h"
#include "epServerConf.h"

namespace epse
{
	/*! 
	@class BasePacketParser epBasePacketParser.h
	@brief A class for Base Packet Parser.
	*/
	class EP_SERVER_ENGINE BasePacketParser:public BaseServerObject
	{
	public:
		/*!
		Default Constructor

		Initializes the Parser
		@param[in] waitTimeMilliSec the wait time in millisecond for terminating
		@param[in] lockPolicyType The lock policy
		*/
		BasePacketParser(unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the BasePacketParser
		@param[in] b the second object
		*/
		BasePacketParser(const BasePacketParser& b);

		/*!
		Default Destructor

		Destroy the Parser
		*/
		virtual ~BasePacketParser();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BasePacketParser & operator=(const BasePacketParser&b);

		/*!
		Send the packet to the client
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@return sent byte size
		@remark return -1 if error occurred
		*/
		int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE);

		/*!
		Parse the given packet and do relevant operation
		@remark  Subclasses must implement this
		@param[in] packet the packet to parse
		*/
		virtual void ParsePacket(const Packet &packet)=0;


		/*!
		Get the owner object of this parser object.
		@return the pointer to the owner object.
		*/
		BaseServerSendObject *GetOwner() const;

		/*!
		Return the packet received.
		@return the packet received.
		*/
		const Packet* GetPacketReceived() const;


		
	protected:

		/// Thread Stop Event
		/// @remark if this is raised, the thread should quickly stop.
		epl::EventEx m_threadStopEvent;

	private:	
		friend class BaseClient;
		friend class BaseServerWorker;
		friend class BaseClientUDP;
		friend class BaseServerWorkerUDP;

		/*! 
		@struct PacketPassUnit epBasePacketParser.h
		@brief A class for Packet Passing Unit for Packet Parsing Thread.
		*/
		struct PacketPassUnit{
			/// BaseServerWorkerEx Object
			BaseServerSendObject *m_owner;
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
		virtual void execute();

		/*!
		Reset Parser
		*/
		void resetParser();


	private:

        /// Owner
		BaseServerSendObject *m_owner;



		/// Packet received
		Packet * m_packetReceived;

		/// Lock Policy
		epl::LockPolicy m_lockPolicy;

		/// general lock
		epl::BaseLock *m_generalLock;

	};

}

#endif //__EP_BASE_PACKET_PARSER_H__