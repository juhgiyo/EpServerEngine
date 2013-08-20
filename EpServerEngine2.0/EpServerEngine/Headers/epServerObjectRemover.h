/*! 
@file epServerObjectRemover.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 20, 2012
@brief Server Object Remover Interface
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

An Interface for Server Object Remover.

*/
#ifndef __EP_SERVER_OBJECT_REMOVER_H__
#define __EP_SERVER_OBJECT_REMOVER_H__

#include "epServerEngine.h"
#include "epBaseServerObject.h"
#include <queue>


using namespace std;


namespace epse{

	/*! 
	@class ServerObjectRemover epServerObjectRemover.h
	@brief A class for Server Object Remover.
	*/
	class EP_SERVER_ENGINE ServerObjectRemover:protected epl::Thread, protected SmartObject{

	private:
		friend class ServerObjectList;
		friend class ParserList;
		/*!
		Default Constructor

		Initializes the Remover
		@param[in] waitTimeMilliSec the wait time in millisecond for terminating
		@param[in] lockPolicyType The lock policy
		*/
		ServerObjectRemover(unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the Remover
		@param[in] b the second object
		*/
		ServerObjectRemover(const ServerObjectRemover& b);
		/*!
		Default Destructor

		Destroy the List
		*/
		virtual ~ServerObjectRemover();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		ServerObjectRemover & operator=(const ServerObjectRemover&b);
		
		/*!
		Set the wait time for the thread termination
		@param[in] milliSec the time for waiting in millisecond
		*/
		void SetWaitTime(unsigned int milliSec);

		/*!
		Get the wait time for the parser thread termination
		@return the current time for waiting in millisecond
		*/
		unsigned int GetWaitTime();

		/*!
		Push the new object to the list
		@param[in] obj the object to push in
		*/
		void Push(BaseServerObject* obj);

	private:
		/*!
		Stop the Loop Function
		*/
		void stopRemover();
		/*!
		Release Loop Function
		*/
		virtual void execute() ;
	private:
	
		/// wait time in millisecond for terminating thread
		unsigned int m_waitTime;

		/// list lock
		epl::BaseLock *m_listLock;

		/// stop lock
		epl::BaseLock *m_stopLock;

		/// parser thread list
		queue<BaseServerObject*> m_objectList;

		/// Lock Policy
		epl::LockPolicy m_lockPolicy;

		/// Thread Stop Event
		/// @remark if this is raised, the thread should quickly stop.
		epl::EventEx m_threadStopEvent;

	};
	
}


#endif //__EP_SERVER_OBJECT_REMOVER_H__