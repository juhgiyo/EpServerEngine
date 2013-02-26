/*! 
@file epBaseServerObject.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 20, 2012
@brief Base Server Object Interface
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

An Interface for Base Server Object.

*/
#ifndef __EP_BASE_SERVER_OBJECT_H__
#define __EP_BASE_SERVER_OBJECT_H__

#include "epServerEngine.h"
#include "epServerConf.h"

namespace epse{

	/*! 
	@class BaseServerObject epBaseServerObject.h
	@brief A class for Base Server Object.
	*/
	class EP_SERVER_ENGINE BaseServerObject:public epl::SmartObject, protected epl::Thread{
		
	public:
		/*!
		Default Constructor

		Initializes the Object
		@param[in] waitTimeMilliSec wait time for Thread to terminate
		@param[in] lockPolicyType The lock policy
		*/
		BaseServerObject(unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);


		/*!
		Default Copy Constructor

		Initializes the Object
		@param[in] b the second object
		*/
		BaseServerObject(const BaseServerObject& b);
		/*!
		Default Destructor

		Destroy the Object
		*/
		virtual ~BaseServerObject();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseServerObject & operator=(const BaseServerObject&b);
		
		/*!
		Set the wait time for the thread termination
		@param[in] milliSec the time for waiting in millisecond
		*/
		virtual void SetWaitTime(unsigned int milliSec);
		
		/*!
		Get the wait time for the parser thread termination
		@return the current time for waiting in millisecond
		*/
		unsigned int GetWaitTime();
		


		
	private:
		friend class ServerObjectList;
		friend class ParserList;
		friend class BasePacketParser;

		friend class BaseClient;
		friend class BaseServer;
		friend class BaseServerWorker;
		friend class BaseClientUDP;
		friend class BaseServerUDP;
		friend class BaseServerWorkerUDP;


		/*!
		Set Synchronous Policy
		@param[in] syncPolicy the synchronous policy to set
		*/
		void setSyncPolicy(SyncPolicy syncPolicy);

		/*!
		Set Container
		@param[in] container the new container for this object
		*/
		void setContainer(ServerObjectList *container);

		/*!
		Remove self from the container
		@return true if successfully removed otherwise false
		*/
		bool removeSelfFromContainer();
	private:

		/// Synchronous Policy
		SyncPolicy m_syncPolicy;

		/// Lock Policy
		LockPolicy m_lockPolicy;
		/// Wait Time in Milliseconds
		unsigned int m_waitTime;

		/// Container
		ServerObjectList *m_container;

		/// container lock
		epl::BaseLock *m_containerLock;
	};
}


#endif //__EP_BASE_SERVER_OBJECT_H__