/*! 
@file epClientInterfaces.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Client Interfaces
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

An Interface for Client Interfaces.

*/
#ifndef __EP_CLIENT_INTERFACES_H__
#define __EP_CLIENT_INTERFACES_H__

#include "epServerEngine.h"
#include "epPacket.h"

namespace epse{
	/*! 
	@class ClientCallbackInterface epClientInterfaces.h
	@brief A class for Client Callback Interface.
	*/
	class EP_SERVER_ENGINE ClientCallbackInterface{
	public:
		virtual void OnReceived(const Packet&recievedPacket)=0;
		virtual void OnDisconnect(){}
	};
}

#endif //__EP_CLIENT_INTERFACES_H__