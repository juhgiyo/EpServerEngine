/*! 
@file epIocpServerProcessor.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief IOCP Server Processor Interface
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

An Interface for IOCP Server Processor.

*/
#ifndef __EP_IOCP_SERVER_PROCESSOR_H__
#define __EP_IOCP_SERVER_PROCESSOR_H__

#include "epServerEngine.h"
namespace epse{
		/*! 
	@class IocpServerProcessor epIocpServerProcessor.h
	@brief A class for IOCP SERVER Processor.
	*/
	class EP_SERVER_ENGINE IocpServerProcessor:public BaseJobProcessor{

	public:
		/*!
		Process the job given, subclasses must implement this function.
		@param[in] workerThread The worker thread which called the DoJob.
		@param[in] data The job given to this object.
		*/
		virtual void DoJob(BaseWorkerThread *workerThread,  BaseJob* const data);


	protected:
		/*!
		Handles when Job Status Changed
		Subclass should overwrite this function!!
		@param[in] status The Status of the Job
		*/
		virtual void handleReport(const JobProcessorStatus status);
	};
}


#endif //__EP_IOCP_SERVER_PROCESSOR_H__