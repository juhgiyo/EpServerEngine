/*! 
@file epBaseServerSendObject.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 20, 2012
@brief Base Server Send Object Interface
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

An Interface for Base Server Send Object.

*/
#ifndef __EP_BASE_SERVER_SEND_OBJECT_H__
#define __EP_BASE_SERVER_SEND_OBJECT_H__

#include "epServerEngine.h"
#include "epBaseServerObject.h"
namespace epse{

	/*! 
	@class BaseServerSendObject epBaseServerSendObject.h
	@brief A class for Base Server Send Object.
	*/
	class EP_SERVER_ENGINE BaseServerSendObject:public BaseServerObject{
	public:
			/*!
		Default Constructor

		Initializes the Object
		@param[in] waitTimeMilliSec wait time for Thread to terminate
		@param[in] lockPolicyType The lock policy
		*/
		BaseServerSendObject(unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY):BaseServerObject(waitTimeMilliSec,lockPolicyType)
		{
		}

		/*!
		Default Copy Constructor

		Initializes the Object
		@param[in] b the second object
		*/
		BaseServerSendObject(const BaseServerObject& b):BaseServerObject(b)
		{
		}
		/*!
		Default Destructor

		Destroy the Object
		*/
		virtual ~BaseServerSendObject(){}

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseServerSendObject & operator=(const BaseServerSendObject&b)
		{
			if(this!=&b)
			{
				BaseServerObject::operator=(b);
			}
			return *this;
		}

		/*!
		Send the given packet to relevance
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@return sent byte size
		@remark  Subclasses must implement this
		*/
		virtual int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE)=0;
	};
}


#endif //__EP_BASE_SERVER_SEND_OBJECT_H__