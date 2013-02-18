/*! 
@file epPacketContainer.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com> 
		<http://github.com/juhgiyo/epserverengine>
@date March 27, 2012
@brief Packet Container Interface
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

An Interface for Packet Container with variable length array.

@section SPECIAL THANKS TO
 
Sang Yong Yoon <flyingshiri@gmail.com>

*/
#ifndef __EP_PACKET_CONTAINER_H__
#define __EP_PACKET_CONTAINER_H__

#include "epServerEngine.h"

namespace epse
{
	/*! 
	@class PacketContainer epPacketContainer.h
	@brief A class for Packet Container with variable length array.
	*/
	template<typename PacketStruct, typename ArrayType=char>
	class PacketContainer
	{
	public:
		/*!
		Default Constructor

		Initializes given size of array of ArrayType addition to the PacketStruct.
		@param[in] arraySize the size of array addition to PacketStruct
		@param[in] shouldAllocate flag for the allocation of memory for itself
		@param[in] lockPolicyType The lock policy
		*/
		PacketContainer(unsigned int arraySize=0, bool shouldAllocate=true, epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);
		

		/*!
		Default Constructor

		Initializes given size of array of ArrayType addition to the PacketStruct.
		@param[in] packet the packet to copy from
		@param[in] arraySize the size of array addition to PacketStruct
		@param[in] shouldAllocate flag for the allocation of memory for itself
		@param[in] lockPolicyType The lock policy
		*/
		PacketContainer(const PacketStruct & packet, unsigned int arraySize=0, bool shouldAllocate=true, epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);
		
		/*!
		Default Constructor

		Initializes PacketContainer with the raw data given.
		@param[in] rawData the rawData to copy from
		@param[in] byteSize the byte size of raw data given.
		@param[in] shouldAllocate flag for the allocation of memory for itself
		@param[in] lockPolicyType The lock policy
		*/
		PacketContainer(const void * rawData, unsigned int byteSize, bool shouldAllocate=true, epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Copy Constructor

		Copy the given Packet Container to this Packet Container.
		@param[in] orig the original PacketContainer
		*/
		PacketContainer(const PacketContainer& orig);
		
		/*!
		Default Destructor

		Destroy the Packet
		*/
		virtual ~PacketContainer();
				

		/*!
		Get a pointer to the packet
		@return the pointer to the packet
		*/
		PacketStruct *GetPacketPtr() const;
		

		/*!
		Get the flag whether memory is allocated or not
		@return true if the memory is allocated by this object, otherwise false
		*/
		bool IsAllocated() const;
		
		/*!
		Set the packet container according to the packet given
		@param[in] packet the packet to copy from
		@param[in] arraySize the size of variable length array of given packet
		*/
		void SetPacket(const PacketStruct & packet, unsigned int arraySize=0);

		/*!
		Set the packet container from the raw data
		@param[in] rawData the rawData to copy from
		@param[in] byteSize the byte size of raw data given.
		@return true if successful otherwise false
		*/
		bool SetPacket(const void * rawData, unsigned int byteSize);

		/*!
		Get the pointer to the array
		@return the pointer to the array
		*/
		ArrayType* GetArray() const;

		/*!
		Set the array with given array
		@param[in] arr the array to copy from
		@param[in] arraySize the size of given array
		@param[in] offset the offset for start position of copying
		@return true if successful otherwise false
		*/
		bool SetArray(const ArrayType *arr,unsigned int arraySize, unsigned int offset=0);
		
		/*!
		Get the length of the array
		@return the length of the array
		*/
		unsigned int GetArrayLength() const;

		/*!
		Get the total byte size of the packet including the array
		@return the total byte size of the packet including the array
		*/
		unsigned int GetPacketByteSize() const;

		/*!
		Change the size of the array
		* the arrSize must be larger than current array size.
		* Should NOT be used when m_isAllocated is false.
		@param[in] arrSize the new size of the array
		@return true if successful otherwise false
		*/
		bool SetArraySize(unsigned int arrSize);

		/*!
		return the reference to the array element at given index
		* the arrSize must be smaller than current array size.
		@param[in] index the index of the array to get the array element
		@return the reference to the array element at given index
		*/
		ArrayType & operator[](unsigned int index);

		/*!
		return the reference to the array element at given index
		* the arrSize must be smaller than current array size.
		@param[in] index the index of the array to get the array element
		@return the reference to the array element at given index
		*/
		const ArrayType & operator[](unsigned int index) const;

		/*!
		Copy the given Packet Container b to this Packet Container.
		@param[in] b the PacketContainer to copy from
		@return this object
		*/
		PacketContainer& operator =(const PacketContainer& b);

		/*!
		Copy the given Packet b to this Packet Container.
		* Should NOT be used when m_isAllocated is false.
		@param[in] b the Packet to copy from
		@return this object
		*/
		PacketContainer& operator =(const PacketStruct& b);
			

	private:

		/*!
		Actual change the size of the array
		* the arrSize must be larger than current array size.
		* Should NOT be used when m_isAllocated is false.
		@param[in] arrSize the new size of the array
		@return true if successful otherwise false
		*/
		bool setArraySize(unsigned int arrSize);

		/*!
		@struct PacketContainerStruct epPacketContainer.h
		@brief the actual Packet Container Class.	
		*/
		struct PacketContainerStruct
		{
			/// Original Packet
			PacketStruct m_packet;
			/// Dynamic Array of the Packet
			ArrayType m_array[0];
		};
		/// length of the array
		unsigned int m_length;
		/// pointer to the actual packet container
		PacketContainerStruct *m_packetContainer;
		/// flag whether memory is allocated in this object or now
		bool m_isAllocated;
		/// lock
		epl::BaseLock *m_packetContainerLock;
		/// Lock Policy
		epl::LockPolicy m_lockPolicy;
	};




	template<typename PacketStruct, typename ArrayType>
	PacketContainer<PacketStruct,ArrayType>::PacketContainer(unsigned int arraySize, bool shouldAllocate, epl::LockPolicy lockPolicyType)
	{
		if(shouldAllocate)
		{
			m_packetContainer=reinterpret_cast<PacketContainerStruct*>(EP_Malloc(sizeof(PacketContainerStruct) + (arraySize*sizeof(ArrayType)) ));
			EP_ASSERT(m_packetContainer);
			m_length=arraySize;
		}
		else
		{
			m_packetContainer=NULL;
			m_length=arraySize;
		}
		m_isAllocated=shouldAllocate;
		m_lockPolicy=lockPolicyType;
		switch(lockPolicyType)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_packetContainerLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_packetContainerLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_packetContainerLock=EP_NEW epl::NoLock();
			break;
		default:
			m_packetContainerLock=NULL;
			break;
		}
	}

	template<typename PacketStruct, typename ArrayType>
	PacketContainer<PacketStruct,ArrayType>::PacketContainer(const PacketStruct & packet, unsigned int arraySize, bool shouldAllocate, epl::LockPolicy lockPolicyType)
	{
		if(shouldAllocate)
		{
			m_packetContainer=reinterpret_cast<PacketContainerStruct*>( EP_Malloc(sizeof(PacketContainerStruct) + (arraySize*sizeof(ArrayType)) ) );
			EP_ASSERT(m_packetContainer);
			epl::System::Memcpy(m_packetContainer,&packet,sizeof(PacketContainerStruct) + (arraySize*sizeof(ArrayType)));
			m_length=arraySize;
		}
		else
		{
			m_packetContainer=reinterpret_cast<PacketContainerStruct*>(&packet);
			m_length=arraySize;
		}
		m_isAllocated=shouldAllocate;
		m_lockPolicy=lockPolicyType;
		switch(lockPolicyType)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_packetContainerLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_packetContainerLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_packetContainerLock=EP_NEW epl::NoLock();
			break;
		default:
			m_packetContainerLock=NULL;
			break;
		}
	}

	template<typename PacketStruct, typename ArrayType>
	PacketContainer<PacketStruct,ArrayType>::PacketContainer(const void * rawData, unsigned int byteSize, bool shouldAllocate, epl::LockPolicy lockPolicyType)
	{


		m_packetContainer=NULL;
		m_isAllocated=shouldAllocate;
		if(byteSize<sizeof(PacketStruct))
		{
			epl::EpTString errMsg;
			epl::System::STPrintf(errMsg,_T("byteSize is smaller than PacketStruct size.\r\nbyteSize must be greater than sizeof(PacketStruct)=%d.\r\nbyteSize = %d\r\n"),sizeof(PacketStruct),byteSize);
			EP_ASSERT_EXPR(byteSize>=sizeof(PacketStruct),errMsg);
		}


		if(m_isAllocated)
		{
			m_packetContainer=reinterpret_cast<PacketContainerStruct*>( EP_Malloc(byteSize) );
			EP_ASSERT(m_packetContainer);
			epl::System::Memcpy(m_packetContainer,rawData,byteSize);
			m_length=(byteSize-sizeof(PacketContainerStruct))/sizeof(ArrayType);
		}
		else
		{
			m_packetContainer=reinterpret_cast<PacketContainerStruct*>(const_cast<void*>(rawData));
			m_length=(byteSize-sizeof(PacketContainerStruct))/sizeof(ArrayType);
		}
		m_lockPolicy=lockPolicyType;
		switch(lockPolicyType)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_packetContainerLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_packetContainerLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_packetContainerLock=EP_NEW epl::NoLock();
			break;
		default:
			m_packetContainerLock=NULL;
			break;
		}
	}


	template<typename PacketStruct, typename ArrayType>
	PacketContainer<PacketStruct,ArrayType>::PacketContainer(const PacketContainer<PacketStruct,ArrayType>& orig)
	{	
		if(orig.m_isAllocated)
		{
			m_packetContainer=reinterpret_cast<PacketContainerStruct*>( EP_Malloc(sizeof(PacketContainerStruct) + (orig.m_length*sizeof(ArrayType)) ) );
			EP_ASSERT(m_packetContainer);
			m_packetContainer->m_packet=orig.m_packetContainer->m_packet;
			m_length=orig.m_length;
			for(int trav=0;trav<m_length;trav++)
			{
				m_packetContainer->m_array[trav]=orig.m_packetContainer->m_array[trav];
			}
		}
		else
		{
			m_packetContainer=orig.m_packetContainer;
			m_length=orig.m_length;
		}
		m_isAllocated=orig.m_isAllocated;
		m_lockPolicy=orig.m_lockPolicy;
		switch(m_lockPolicy)
		{
		case epl::LOCK_POLICY_CRITICALSECTION:
			m_packetContainerLock=EP_NEW epl::CriticalSectionEx();
			break;
		case epl::LOCK_POLICY_MUTEX:
			m_packetContainerLock=EP_NEW epl::Mutex();
			break;
		case epl::LOCK_POLICY_NONE:
			m_packetContainerLock=EP_NEW epl::NoLock();
			break;
		default:
			m_packetContainerLock=NULL;
			break;
		}
	}

	template<typename PacketStruct, typename ArrayType>
	PacketContainer<PacketStruct,ArrayType>::~PacketContainer()
	{
		m_packetContainerLock->Lock();
		if(m_isAllocated && m_packetContainer)
			EP_Free(m_packetContainer);	
		m_packetContainerLock->Unlock();
		if(m_packetContainerLock)
			EP_DELETE m_packetContainerLock;
	}
	

	template<typename PacketStruct, typename ArrayType>
	PacketStruct *PacketContainer<PacketStruct,ArrayType>::GetPacketPtr() const
	{
		return reinterpret_cast<PacketStruct*>(m_packetContainer);
	}

	template<typename PacketStruct, typename ArrayType>
	bool PacketContainer<PacketStruct,ArrayType>::IsAllocated() const
	{
		return m_isAllocated;
	}
	
	template<typename PacketStruct, typename ArrayType>
	void PacketContainer<PacketStruct,ArrayType>::SetPacket(const PacketStruct & packet, unsigned int arraySize)
	{
		epl::LockObj lock(m_packetContainerLock);
		if(m_isAllocated && m_packetContainer)
			EP_Free(m_packetContainer);
		m_packetContainer=NULL;

		if(m_isAllocated)
		{
			m_packetContainer=reinterpret_cast<PacketContainerStruct*>( EP_Malloc(sizeof(PacketContainerStruct) + (arraySize*sizeof(ArrayType)) ) );
			EP_ASSERT(m_packetContainer);
			epl::System::Memcpy(m_packetContainer,&packet,sizeof(PacketContainerStruct) + (arraySize*sizeof(ArrayType)));
			m_length=arraySize;
		}
		else
		{
			m_packetContainer=reinterpret_cast<PacketContainerStruct*>(&packet);
			m_length=arraySize;
		}
	}

	template<typename PacketStruct, typename ArrayType>
	bool PacketContainer<PacketStruct,ArrayType>::SetPacket(const void * rawData, unsigned int byteSize)
	{
		epl::LockObj lock(m_packetContainerLock);
		if(byteSize<sizeof(PacketStruct))
		{
			epl::EpTString errMsg;
			epl::System::STPrintf(errMsg,_T("byteSize is smaller than PacketStruct size.\r\nbyteSize must be greater than sizeof(PacketStruct)=%d.\r\nbyteSize = %d\r\n"),sizeof(PacketStruct),byteSize);
			EP_ASSERT_EXPR(byteSize>=sizeof(PacketStruct),errMsg);
		}


		if(m_isAllocated && m_packetContainer)
			EP_Free(m_packetContainer);
		m_packetContainer=NULL;

		if(m_isAllocated)
		{
			m_packetContainer=reinterpret_cast<PacketContainerStruct*>( EP_Malloc(byteSize) );
			EP_ASSERT(m_packetContainer);
			epl::System::Memcpy(m_packetContainer,rawData,byteSize);
			m_length=(byteSize-sizeof(PacketContainerStruct))/sizeof(ArrayType);
		}
		else
		{
			m_packetContainer=reinterpret_cast<PacketContainerStruct*>(const_cast<void*>(rawData));
			m_length=(byteSize-sizeof(PacketContainerStruct))/sizeof(ArrayType);
		}
		return true;
	}

	template<typename PacketStruct, typename ArrayType>
	ArrayType* PacketContainer<PacketStruct,ArrayType>::GetArray() const
	{
		if(m_packetContainer && m_length>0)
			return m_packetContainer->m_array;
		return NULL;
	}

	template<typename PacketStruct, typename ArrayType>
	bool PacketContainer<PacketStruct,ArrayType>::SetArray(const ArrayType *arr,unsigned int arraySize, unsigned int offset)
	{
		epl::LockObj lock(m_packetContainerLock);
		if(m_isAllocated)
		{
			if(m_length<arraySize+offset)
			{
				if(!setArraySize(arraySize+offset))
					return false;
			}
			for(int trav=0;trav<arraySize;trav++)
			{
				m_packetContainer->m_array[offset+trav]=arr[trav];
			}			
			return true;
		}
		else
		{
			if(m_length<arraySize+offset)
			{
				return false;
			}
			for(int trav=0;trav<arraySize;trav++)
			{
				m_packetContainer->m_array[offset+trav]=arr[trav];
			}			
			return true;
		}
	}

	template<typename PacketStruct, typename ArrayType>
	unsigned int PacketContainer<PacketStruct,ArrayType>::GetArrayLength() const
	{
		return m_length;
	}

	template<typename PacketStruct, typename ArrayType>
	unsigned int PacketContainer<PacketStruct,ArrayType>::GetPacketByteSize() const
	{
		return (sizeof(PacketStruct)+(sizeof(ArrayType)*m_length));
	}

	template<typename PacketStruct, typename ArrayType>
	bool PacketContainer<PacketStruct,ArrayType>::SetArraySize(unsigned int arrSize)
	{
		epl::LockObj lock(m_packetContainerLock);
		return setArraySize(arrSize);
	}

	template<typename PacketStruct, typename ArrayType>
	ArrayType & PacketContainer<PacketStruct,ArrayType>::operator[](unsigned int index)
	{
		EP_ASSERT_EXPR(m_packetContainer,_T("Actual packet is NULL."));
		EP_ASSERT(index<m_length);
		return m_packetContainer->m_array[index];
	}

	template<typename PacketStruct, typename ArrayType>
	const ArrayType & PacketContainer<PacketStruct,ArrayType>::operator[](unsigned int index) const
	{
		EP_ASSERT_EXPR(m_packetContainer,_T("Actual packet is NULL."));
		EP_ASSERT(index<m_length);
		return m_packetContainer->m_array[index];
	}

	template<typename PacketStruct, typename ArrayType>
	PacketContainer<PacketStruct,ArrayType>& PacketContainer<PacketStruct,ArrayType>::operator =(const PacketContainer<PacketStruct,ArrayType>& b)
	{
		if(this!=&b)
		{
			epl::LockObj lock(m_packetContainerLock);
			if(m_isAllocated && m_packetContainer)
				EP_Free(m_packetContainer);
			m_packetContainer=NULL;
			if(b.m_isAllocated)
			{
				m_packetContainer=reinterpret_cast<PacketContainerStruct*>( EP_Malloc(sizeof(PacketContainerStruct) + (b.m_length*sizeof(ArrayType)) ) );
				EP_ASSERT(m_packetContainer);
				m_packetContainer->m_packet=b.m_packetContainer->m_packet;
				m_length=b.m_length;
				for(int trav=0;trav<m_length;trav++)
				{
					m_packetContainer->m_array[trav]=b.m_packetContainer->m_array[trav];
				}
			}
			else
			{
				m_packetContainer=b.m_packetContainer;
				m_length=b.m_length;
			}
			m_isAllocated=b.m_isAllocated;
		}
		return *this;
	}

	template<typename PacketStruct, typename ArrayType>
	PacketContainer<PacketStruct,ArrayType>& PacketContainer<PacketStruct,ArrayType>::operator =(const PacketStruct& b)
	{
		epl::LockObj lock(m_packetContainerLock);
		if(m_isAllocated)
			m_packetContainer->m_packet=b;
		else
			EP_ASSERT_EXPR(0,_T("This Function Should NOT be used when m_isAllocated is false.\r\nInstead USE SetPacket Function."));
		return *this;
	}
	

	template<typename PacketStruct, typename ArrayType>
	bool PacketContainer<PacketStruct,ArrayType>::setArraySize(unsigned int arrSize)
	{
		if(m_isAllocated)
		{
			if(m_length==arrSize)
				return true;

			if(arrSize<=m_length)
			{
				epl::EpTString errMsg;
				epl::System::STPrintf(errMsg,_T("Given size = %d is smaller than the original = %d.\r\nNew array size must be (greater than/equal to) original array size."),arrSize,m_length);
				EP_ASSERT_EXPR(arrSize>=m_length,errMsg);
			}
			
			m_packetContainer=reinterpret_cast<PacketContainerStruct*>(EP_Realloc(m_packetContainer,sizeof(PacketContainerStruct)+ (arrSize*sizeof(ArrayType))));
			epl::System::Memset(((char*)m_packetContainer)+sizeof(PacketContainerStruct)+ (m_length*sizeof(ArrayType)),0,((arrSize-m_length)*sizeof(ArrayType)));
			EP_ASSERT(m_packetContainer);
			m_length=arrSize;
			return true;
		}
		else
		{
			EP_ASSERT_EXPR(0,_T("This Function Should NOT be used when m_isAllocated is false.\r\nInstead USE SetPacket Function."));
			return false;
		}
	}





}
#endif //__EP_PACKET_CONTAINER_H__