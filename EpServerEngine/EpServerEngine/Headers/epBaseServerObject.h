/*! 
@file epBaseServerObject.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 20, 2012
@brief Base Server Object Interface
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