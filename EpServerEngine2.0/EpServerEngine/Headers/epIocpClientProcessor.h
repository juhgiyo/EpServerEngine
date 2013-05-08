/*! 
@file epIocpClientProcessor.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief IOCP Client Processor Interface
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

An Interface for IOCP Client Processor.

*/
#ifndef __EP_IOCP_CLIENT_PROCESSOR_H__
#define __EP_IOCP_CLIENT_PROCESSOR_H__

#include "epServerEngine.h"
namespace epse{
		/*! 
	@class IocpClientProcessor epIocpClientProcessor.h
	@brief A class for IOCP Client Processor.
	*/
	class EP_SERVER_ENGINE IocpClientProcessor:public BaseJobProcessor{

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


#endif //__EP_IOCP_CLIENT_PROCESSOR_H__