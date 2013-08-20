/*! 
@file epParserList.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date January 31, 2013
@brief Parser List Interface
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
	private:
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

		/// Stop Parse Lock
		epl::BaseLock * m_stopLock;
		/// Synchronous Policy
		SyncPolicy m_syncPolicy;
		/// Lock Policy
		LockPolicy m_lockPolicy;

		/// Thread Stop Event
		/// @remark if this is raised, the thread should quickly stop.
		epl::EventEx m_threadStopEvent;

	};
	
}


#endif //__EP_SERVER_OBJECT_LIST_H__