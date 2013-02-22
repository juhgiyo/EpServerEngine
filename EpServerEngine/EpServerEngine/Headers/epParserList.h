/*! 
@file epParserList.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date January 31, 2013
@brief Parser List Interface
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

An Interface for Parser List.

*/
#ifndef __EP_PARSER_LIST_H__
#define __EP_PARSER_LIST_H__

#include "epServerEngine.h"
#include "epBasePacketParser.h"
#include "epServerObjectList.h"


using namespace std;


namespace epse{

	/*! 
	@class ParserList epParserList.h
	@brief A class for Parser List.
	*/
	class EP_SERVER_ENGINE ParserList:protected ServerObjectList, protected epl::Thread, protected SmartObject {
		friend class BaseClient;
		friend class BaseClientUDP;
		friend class BaseServer;
		friend class BaseServerWorker;
		friend class BaseServerUDP;
		friend class BaseServerWorkerUDP;
	protected:
		/*!
		Default Constructor

		Initializes the List
		@param[in] syncPolicy The sync policy
		@param[in] waitTimeMilliSec the wait time in millisecond for terminating
		@param[in] lockPolicyType The lock policy
		*/
		ParserList(SyncPolicy syncPolicy=SYNC_POLICY_ASYNCHRONOUS, unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the List
		@param[in] b the second object
		*/
		ParserList(const ParserList& b);
		/*!
		Default Destructor

		Destroy the List
		*/
		virtual ~ParserList();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		ParserList & operator=(const ParserList&b);

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
		Remove all object which its thread is terminated
		@remark it also releases the object
		@remark This function removes object only for SYNC_POLICY_ASYNCHRONOUS policy.
		*/
		virtual void RemoveTerminated();

		/*!
		Start Parse
		@return true if successfully started otherwise false
		*/
		bool StartParse();

		/*!
		Stop Parse
		*/
		void StopParse();

		/*!
		Push the new object to the list
		@param[in] obj the object to push in
		*/
		virtual void Push(BaseServerObject* obj);
	private:
		/*!
		Set the Parser List's SyncPolicy
		@param[in] syncPolicy synchronous policy
		*/
        void setSyncPolicy(SyncPolicy syncPolicy);

		/*!
		Parse Loop Function
		*/
		virtual void execute() ;

	private:

		/// Thread Terminate
		bool m_shouldTerminate;

		/// wait time in millisecond for terminating thread
		unsigned int m_waitTime;

		/// Synchronous Policy
		SyncPolicy m_syncPolicy;

		/// Event
		epl::EventEx m_event;

	};
	
}


#endif //__EP_SERVER_OBJECT_LIST_H__