/*! 
@file epServerObjectList.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 20, 2012
@brief Server Object List Interface
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

An Interface for Server Object List.

*/
#ifndef __EP_SERVER_OBJECT_LIST_H__
#define __EP_SERVER_OBJECT_LIST_H__

#include "epServerEngine.h"
#include "epBaseServerObject.h"
#include "epServerObjectRemover.h"
#include <vector>
#include "epPacket.h"

using namespace std;


namespace epse{

	/*! 
	@class ServerObjectList epServerObjectList.h
	@brief A class for Server Object List.
	*/
	class EP_SERVER_ENGINE ServerObjectList{

	protected:
		friend class BaseClient;
		friend class BaseClientUDP;
		friend class BaseServer;
		friend class BaseServerWorker;
		friend class BaseServerUDP;
		friend class BaseServerWorkerUDP;
		
		friend class BaseServerObject;
		/*!
		Default Constructor

		Initializes the List
		@param[in] waitTimeMilliSec the wait time in millisecond for terminating
		@param[in] lockPolicyType The lock policy
		*/
		ServerObjectList(unsigned int waitTimeMilliSec=WAITTIME_INIFINITE, epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the List
		@param[in] b the second object
		*/
		ServerObjectList(const ServerObjectList& b);
		/*!
		Default Destructor

		Destroy the List
		*/
		virtual ~ServerObjectList();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		ServerObjectList & operator=(const ServerObjectList&b);

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
		Remove the given object from the list
		@param[in] serverObj the server object to remove
		@remark it also releases the object
		*/
		virtual bool Remove(const BaseServerObject* serverObj);

	
		/*!
		Remove all object
		@remark it also releases the object
		*/
		void Clear();

		/*!
		Push the new object to the list
		@param[in] obj the object to push in
		*/
		virtual void Push(BaseServerObject* obj);
		
		/*!
		Returns the list in vector
		@return the list of the objects
		*/
		vector<BaseServerObject*> GetList() const;

		/*!
		Returns the number of element in the list
		@return the number of element in the list
		*/
		size_t Count() const;

		/*!
		Do the action given by input function for all elements
		@param[in] DoFunc the action for each element
		@param[in] argCount the number of arguments
		*/
		void Do(void (__cdecl *DoFunc)(BaseServerObject*,unsigned int,va_list),unsigned int argCount,...);

		/*!
		Do the action given by input function for all elements
		@param[in] DoFunc the action for each element
		@param[in] argCount the number of arguments
		@param[in] args the argument list
		*/
		void Do(void (__cdecl *DoFunc)(BaseServerObject*,unsigned int,va_list),unsigned int argCount,va_list args);

		/*!
		Find with given key by comparing with given function
		@param[in] key the key to find
		@param[in] EqualFunc the Compare Function
		@return the found BaseServerObject
		*/
		template <typename T>
		BaseServerObject  *Find(T const & key, bool (__cdecl *EqualFunc)(T const &, const BaseServerObject *))
		{
			epl::LockObj lock(m_listLock);
			vector<BaseServerObject*>::iterator iter;
			for(iter=m_objectList.begin();iter!=m_objectList.end();iter++)
			{
				if(EqualFunc(key,*iter))
				{
					return *iter;
				}
			}
			return NULL;
		}

		/*!
		Wait infinitely for the list size to be decreased
		*/
		void WaitForListSizeDecrease();

	protected:
		/*!
		Reset the list
		*/
		void resetList();
	
		/// list lock
		epl::BaseLock *m_listLock;

		/// parser thread list
		vector<BaseServerObject*> m_objectList;

		/// wait time in millisecond for terminating thread
		/// @remark for ParserList and ServerObjectRemover
		unsigned int m_waitTime;

		/// Lock Policy
		epl::LockPolicy m_lockPolicy;

		/// Object Remover
		ServerObjectRemover m_serverObjRemover;

		epl::EventEx m_sizeEvent;

	};
	
}


#endif //__EP_SERVER_OBJECT_LIST_H__