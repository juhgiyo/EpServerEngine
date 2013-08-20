/*! 
@file epPacket.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Packet Interface
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

An Interface for Packet.

*/
#ifndef __EP_PACKET_H__
#define __EP_PACKET_H__

#include "epServerEngine.h"

namespace epse{

	/*! 
	@class Packet epPacket.h
	@brief A class for Packet.
	*/
	class EP_SERVER_ENGINE Packet:public epl::SmartObject{

	public:
		/*!
		Default Constructor

		Initializes the Packet
		@param[in] packet packet to copy from
		@param[in] byteSize the byte size of the packet given
		@param[in] shouldAllocate flag for the allocation of memory for itself
		@param[in] lockPolicyType The lock policy
		*/
		Packet(const void *packet=NULL, unsigned int byteSize=0, bool shouldAllocate=true, epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the Packet
		@param[in] b the original Packet object
		*/
		Packet(const Packet& b);

		/*!
		Assignment Operator Overloading

		the Packet set as given packet b
		@param[in] b right side of packet
		@return this object
		*/
		Packet & operator=(const Packet&b);

		/*!
		Default Destructor

		Destroy the Packet
		*/
		virtual ~Packet();

		/*!
		Return the currently stored packet byte size
		@return byte size of the holding packet
		*/
		 unsigned int GetPacketByteSize() const;

		/*!
		Get the flag whether memory is allocated or not
		@return true if the memory is allocated by this object, otherwise false
		*/
		bool IsAllocated() const
		{
			return m_isAllocated;
		}

		/*!
		Return the currently holding packet
		@return holding packet
		*/
		const char *GetPacket() const;

		/*!
		Set the packet as given
		@param[in] packet the packet data
		@param[in] packetByteSize the size of packet given
		*/
		void SetPacket(const void* packet, unsigned int packetByteSize);

	private:

		/*!
		Reset Packet
		*/
		void resetPacket();
		/// packet
		char *m_packet;
		/// packet Byte Size
		unsigned int m_packetSize;
		/// flag whether memory is allocated in this object or now
		bool m_isAllocated;
		/// lock
		epl::BaseLock *m_packetLock;
		/// Lock Policy
		epl::LockPolicy m_lockPolicy;
	};
}


#endif //__EP_PACKET_H__