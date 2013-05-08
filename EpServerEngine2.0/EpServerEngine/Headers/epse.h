/*!
@file epse.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 23, 2012
@brief Interface for EP Server Engine
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