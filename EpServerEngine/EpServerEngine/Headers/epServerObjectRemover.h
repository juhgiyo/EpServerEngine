/*! 
@file epServerObjectRemover.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 20, 2012
@brief Server Object Remover Interface
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