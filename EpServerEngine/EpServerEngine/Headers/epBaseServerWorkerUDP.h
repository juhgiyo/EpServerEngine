/*! 
@file epBaseServerWorkerUDP.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base UDP Worker Interface
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

An Interface for Base UDP Server Worker.

*/
#ifndef __EP_BASE_SERVER_WORKER_UDP_H__
#define __EP_BASE_SERVER_WORKER_UDP_H__

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



// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

namespace epse
{
	class BaseServerUDP;
	/*! 
	@class BaseServerWorker epBaseServerWorkerUDP.h
	@brief A class for Base UDP Server Worker.
	*/
	class EP_SERVER_ENGINE BaseServerWorkerUDP:public BaseServerSendObject
	{
		friend class BaseServerUDP;
	public:
		/*!
		Default Constructor

		Initializes the Worker
		@param[in] waitTimeMilliSec wait time for Worker Thread to terminate
		@param[in] lockPolicyType The lock policy
		*/
		BaseServerWorkerUDP(unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the BaseServerWorkerUDP
		@param[in] b the second object
		*/
		BaseServerWorkerUDP(const BaseServerWorkerUDP& b);

		/*!
		Default Destructor

		Destroy the Worker
		*/
		virtual ~BaseServerWorkerUDP();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseServerWorkerUDP & operator=(const BaseServerWorkerUDP&b)
		{
			if(this!=&b)
			{
				epl::LockObj lock(m_lock);
				BaseServerSendObject::operator =(b);
// 				m_clientSocket=b.m_clientSocket;
// 				m_server=b.m_server;
// 
// 				if(m_packet)
// 					m_packet->ReleaseObj();
// 				m_packet=b.m_packet;
// 				if(m_packet)
// 					m_packet->RetainObj();
// 
// 				if(m_parser)
// 					m_parser->ReleaseObj();
// 				m_parser=b.m_parser;
// 				if(m_parser)
// 					m_parser->RetainObj();
// 
// 				if(m_parserList)
// 					m_parserList->ReleaseObj();
// 				m_parserList=m_parserList;
// 				if(m_parserList)
// 					m_parserList->RetainObj();

				m_maxPacketSize=b.m_maxPacketSize;

			}
			return *this;
		}

		/*!
		Send the packet to the client
		@param[in] packet the packet to be sent
		@return sent byte size
		*/
		virtual int Send(const Packet &packet);

		/*!
		Get the maximum packet byte size
		@return the maximum packet byte size
		*/
		unsigned int GetMaxPacketByteSize() const;

	protected:
		/*!
		Return the new packet parser
		@remark Sub-class should implement this to create new parser.
		@remark Client will automatically release this parser.
		@return the new packet parser
		*/
		virtual BasePacketParser* createNewPacketParser()=0;

	private:	
		friend class BaseServerUDP;
		/*!
		Set Parser List for current worker
		@param[in] parserList the parser list to set
		*/
		void setParserList(ParserList *parserList);
	
	private:	


		/*! 
		@struct PacketPassUnit epBaseServerWorkerUDP.h
		@brief A class for Packet Passing Unit for Packet Parsing Thread.
		*/
		struct PacketPassUnit{
			/// BaseServerUDP Object
			BaseServerUDP *m_server;
			/// Packet to parse
			Packet *m_packet;
			/// client socket
			sockaddr m_clientSocket;
		};

		/*!
		thread loop function
		*/
		virtual void execute();

		/*!
		Set the server object as given
		@param[in] server the pointer to the server object
		*/
		void setServer(BaseServerUDP *server);

		/*!
		Set PacketPassUnit
		@param[in] packetPassUnit PacketPassUnit to set
		*/
		void setPacketPassUnit(const PacketPassUnit& packetPassUnit);	
	
		/// client socket
		sockaddr m_clientSocket;

		/// server object
		BaseServerUDP *m_server;

		/// packet received
		Packet *m_packet;

		/// Maximum UDP Datagram byte size
		unsigned int m_maxPacketSize;

		/// general lock 
		epl::BaseLock *m_lock;

		/// Lock Policy
		epl::LockPolicy m_lockPolicy;
		
		/// Parser pointer
		BasePacketParser *m_parser;

		/// parser thread list
		ParserList *m_parserList;
	};

}

#endif //__EP_BASE_SERVER_WORKER_UDP_H__