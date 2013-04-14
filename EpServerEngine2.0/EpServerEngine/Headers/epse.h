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

// Client Side
#include "epBaseClient.h"
#include "epBaseClientManual.h"
#include "epBaseClientUDP.h"
#include "epBaseClientUDPManual.h"

// Server Side
#include "epBaseServer.h"
#include "epBaseServerWorker.h"

// Server Worker
#include "epBaseServerUDP.h"
#include "epBaseServerWorkerUDP.h"

// ServerObject List
#include "epServerObjectList.h"
#include "epParserList.h"
#include "epServerObjectRemover.h"

// General
#include "epBasePacketParser.h"
#include "epBaseServerObject.h"
#include "epBaseServerSendObject.h"
#include "epPacket.h"
#include "epPacketContainer.h"
#include "epServerConf.h"



#endif //__EP_EPSE_H__