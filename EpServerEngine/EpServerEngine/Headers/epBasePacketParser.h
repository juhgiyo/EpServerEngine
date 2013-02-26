/*! 
@file epBasePacketParser.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 20, 2012
@brief Base Packet Parser Interface
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