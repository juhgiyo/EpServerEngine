/*! 
@file epServerEngine.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 23, 2012
@brief EP Server Engine Interface
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

An Interface for EP Server Engine.
*/

#ifndef __EP_SERVER_ENGINE_H__
#define __EP_SERVER_ENGINE_H__

#ifdef EP_SERVER_ENGINE_DLL_EXPORT

#define EP_SERVER_ENGINE __declspec(dllexport)

#else   //EP_SERVER_ENGINE_DLL_EXPORT

#ifdef EP_SERVER_ENGINE_DLL_IMPORT

#define EP_SERVER_ENGINE __declspec(dllimport)

#else   //EP_SERVER_ENGINE_DLL_IMPORT

#define EP_SERVER_ENGINE

#endif  //EP_SERVER_ENGINE_DLL_IMPORT
#endif  //EP_SERVER_ENGINE_DLL_EXPORT

#include "epl.h"
using namespace epl;

#endif //__EP_SERVER_ENGINE_H__