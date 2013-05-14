/*! 
@file epIpcServerInterfaces.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/eplibrary>
@date October 01, 2011
@brief IPC Server Interfaces
@version 2.0

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

An Interface for the IPC Server Interfaces.

*/

#ifndef __EP_IPC_SERVER_INTERFACES_H__
#define __EP_IPC_SERVER_INTERFACES_H__


#include "epLib.h"
#include "epIpcConf.h"

namespace epl
{

	class IpcServerCallbackInterface;
	/*! 
	@struct ServerOps epIpcServerInterfaces.h
	@brief A class for IPC Server Options.
	*/
	struct EP_LIBRARY IpcServerOps{
		/// Callback Object
		IpcServerCallbackInterface *callBackObj;
		
		/// Domain
		TCHAR *domain;
		/// Name of the pipe
		TCHAR *pipeName;
		/// Wait time in millisecond for pipe threads
		unsigned int waitTimeInMilliSec;
		///The maximum possible number of pipe instances
		unsigned int maximumInstances;
		/// read byte size
		unsigned int numOfReadBytes;
		/// write byte size
		unsigned int numOfWriteBytes;

		/*!
		Default Constructor

		Initializes the Server Options
		*/
		IpcServerOps()
		{
			domain=_T(".");
			callBackObj=NULL;
			pipeName=NULL;
			waitTimeInMilliSec=WAITTIME_INIFINITE;
			maximumInstances=PIPE_UNLIMITED_INSTANCES;
			numOfReadBytes=DEFAULT_READ_BUF_SIZE;
			numOfWriteBytes=DEFAULT_WRITE_BUF_SIZE;

		}

		/// Default IPC Server options
		static IpcServerOps defaultIpcServerOps;
	};

	/*! 
	@class IpcServerInterface epIpcServerInterfaces.h
	@brief A class for IPC Server Interface.
	*/
	class EP_LIBRARY IpcServerInterface{

		/*!
		Get the pipe name of server
		@return the pipe name in string
		*/
		virtual epl::EpTString GetFullPipeName() const=0;

		/*!
		Get the Maximum Instances of server
		@return the Maximum Instances
		*/
		virtual unsigned int GetMaximumInstances() const=0;

		/*!
		Set the Callback Object for the server.
		@param[in] callBackObj The Callback Object to set.
		*/
		virtual void SetCallbackObject(IpcServerCallbackInterface *callBackObj)=0;

		/*!
		Get the Callback Object of server
		@return the current Callback Object
		*/
		virtual IpcServerCallbackInterface *GetCallbackObject()=0;

		/*!
		Start the server
		@param[in] ops the server options
		@return true if successfully started otherwise false
		@remark if argument is NULL then previously setting value is used
		*/
		virtual bool StartServer(const IpcServerOps &ops=IpcServerOps::defaultIpcServerOps)=0;
		/*!
		Stop the server
		*/
		virtual void StopServer()=0;

		/*!
		Check if the server is started
		@return true if the server is started otherwise false
		*/
		virtual bool IsServerStarted() const=0;

		/*!
		Terminate all clients' pipe connected.
		*/
		virtual void ShutdownAllClient()=0;

		/*!
		Get the maximum write data byte size
		@return the maximum write data byte size
		*/
		virtual unsigned int GetMaxWriteDataByteSize() const=0;
		/*!
		Get the maximum read data byte size
		@return the maximum read data byte size
		*/
		virtual unsigned int GetMaxReadDataByteSize() const=0;
		
	};

	
	/*! 
	@class IpcInterface epIpcServerInterfaces.h
	@brief A class for IPC Interface.
	*/
	class EP_LIBRARY IpcInterface{
	public:

		/*!
		Write data to the pipe
		@param[in] data the data to write
		@param[in] dataByteSize byte size of the data
		*/
		virtual void Write(char *data,unsigned int dataByteSize)=0;
	
		/*!
		Check if the connection is alive
		@return true if the connection is alive otherwise false
		*/
		virtual bool IsConnectionAlive() const=0;

		/*!
		Kill the connection
		*/
		virtual void KillConnection()=0;

		/*!
		Set the Callback Object for the server.
		@param[in] callBackObj The Callback Object to set.
		*/
		virtual void SetCallbackObject(IpcServerCallbackInterface *callBackObj)=0;

		/*!
		Get the Callback Object of server
		@return the current Callback Object
		*/
		virtual IpcServerCallbackInterface *GetCallbackObject()=0;

	};

	
	/*! 
	@class IpcServerCallbackInterface epIpcServerInterfaces.h
	@brief A class for Server Callback Interface.
	*/
	class EP_LIBRARY IpcServerCallbackInterface{
	public:
		/*!
		When accepted client tries to make connection.
		@param[in] pipe the pipe
		@remark When this function calls, it is right before making connection,<br/>
		        so user can configure the pipe before the connection is actually made.		
		*/
		virtual void OnNewConnection(IpcInterface *pipe){}

		/*!
		Received the data from the client.
		@param[in] pipe the pipe which received the packet
		@param[in] receivedData the received data
		@param[in] receivedDataByteSize the received data byte size
		@param[in] status the status of read
		@param[in] errCode the error code
		*/
		virtual void OnReadComplete(IpcInterface *pipe,const char*receivedData, unsigned int receivedDataByteSize, ReadStatus status, unsigned long errCode)=0;

		/*!
		Received the packet from the client.
		@param[in] pipe the pipe which wrote the packet
		@param[in] writtenDataByteSize the byte size of data written
		@param[in] status the status of write
		@param[in] errCode the error code
		*/
		virtual void OnWriteComplete(IpcInterface *pipe,unsigned int writtenDataByteSize, WriteStatus status, unsigned long errCode){}

		/*!
		The pipe is disconnected.
		@param[in] pipe the pipe, disconnected.
		*/
		virtual void OnDisconnect(IpcInterface *pipe){}
	};


}

#endif //__EP_PIPE_SERVER_INTERFACES_H__