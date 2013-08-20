/*!
@file epse.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 23, 2012
@brief Interface for EP Server Engine
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

An Interface for EP Server Engine
*/
#ifndef __EP_EPSE_H__
#define __EP_EPSE_H__

// General
#include "epServerConf.h"
#include "epPacket.h"
#include "epBaseServerObject.h"
#include "epPacketContainer.h"
#include "epBasePacketProcessor.h"
#include "epServerObjectList.h"
#include "epServerObjectRemover.h"


// Client Side
#include "epClientInterfaces.h"

#include "epBaseTcpClient.h"
#include "epBaseUdpClient.h"
#include "epBaseClient.h"

#include "epClientPacketProcessor.h"
#include "epAsyncTcpClient.h"
#include "epAsyncUdpClient.h"

#include "epSyncTcpClient.h"
#include "epSyncUdpClient.h"

#include "epIocpClientJob.h"
#include "epIocpClientProcessor.h"
#include "epIocpTcpClient.h"
#include "epIocpUdpClient.h"


// Server Side
#include "epServerInterfaces.h"

#include "epBaseSocket.h"
#include "epBaseServer.h"
#include "epBaseTcpSocket.h"
#include "epBaseTcpServer.h"
#include "epBaseUdpSocket.h"
#include "epBaseUdpServer.h"

#include "epServerPacketProcessor.h"
#include "epAsyncTcpServer.h"
#include "epAsyncTcpSocket.h"
#include "epAsyncUdpServer.h"
#include "epAsyncUdpSocket.h"

#include "epSyncTcpServer.h"
#include "epSyncTcpSocket.h"
#include "epSyncUdpServer.h"
#include "epSyncUdpSocket.h"

#include "epIocpServerJob.h"
#include "epIocpServerProcessor.h"
#include "epIocpTcpServer.h"
#include "epIocpTcpSocket.h"
#include "epIocpUdpServer.h"
#include "epIocpUdpSocket.h"

#include "epProxyServerInterfaces.h"
#include "epBaseProxyHandler.h"
#include "epBaseProxyServer.h"
#include "epProxyTcpHandler.h"
#include "epProxyTcpServer.h"
#include "epProxyUdpHandler.h"
#include "epProxyUdpServer.h"


#endif //__EP_EPSE_H__