/*! 
@file epClientPacketProcessor.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 20, 2012
@brief Client Packet Processor Interface
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

An Interface for Client Packet Processor.

*/
#ifndef __EP_CLIENT_PACKET_PROCESSOR_H__
#define __EP_CLIENT_PACKET_PROCESSOR_H__

#include "epServerEngine.h"
#include "epClientInterfaces.h"
#include "epBasePacketProcessor.h"
namespace epse
{
	/*! 
	@class ClientPacketProcessor epClientPacketProcessor.h
	@brief A class for Client Packet Processor.
	*/
	class EP_SERVER_ENGINE ClientPacketProcessor:public BasePacketProcessor
	{
	public:
		/*!
		Default Constructor

		Initializes the Processor
		@param[in] callBackObj the callback object
		@param[in] waitTimeMilliSec the wait time in millisecond for terminating
		@param[in] lockPolicyType The lock policy
		*/
		ClientPacketProcessor(ClientCallbackInterface *callBackObj,unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Destructor

		Destroy the Processor
		*/
		virtual ~ClientPacketProcessor();

	private:
		/*!
		Default Copy Constructor

		Initializes the ClientPacketProcessor
		@param[in] b the second object
		@remark Copy Constructor prohibited
		*/
		ClientPacketProcessor(const ClientPacketProcessor& b):BasePacketProcessor(b)
		{}

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		@remark Copy Operator prohibited
		*/
		ClientPacketProcessor & operator=(const ClientPacketProcessor&b){return *this;}


	private:	
		friend class AsyncTcpClient;
	
		friend class AsyncUdpClient;
	
		/*!
		thread loop function
		*/
		virtual void execute();


	private:

		/// Callback Object
		ClientCallbackInterface *m_callBackObj;


	};

}

#endif //__EP_CLIENT_PACKET_PROCESSOR_H__