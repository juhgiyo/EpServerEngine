/*! 
@file epServerEngine.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date July 23, 2012
@brief EP Server Engine Interface
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

An Interface for EP Server Engine.
*/

#ifndef __EP_SERVER_ENGINE_H__
#define __EP_SERVER_ENGINE_H__

#ifdef EP_SERVER_ENGINE_DLL_EXPORT

#define EP_SERVER_ENGINE __declspec(dllexport)
#ifndef EP_LIBRARY_DLL_IMPORT
#define EP_LIBRARY_DLL_IMPORT
#endif //EP_LIBRARY_DLL_IMPORT

#else   //EP_SERVER_ENGINE_DLL_EXPORT

#ifdef EP_SERVER_ENGINE_DLL_IMPORT

#define EP_SERVER_ENGINE __declspec(dllimport)
#ifndef EP_LIBRARY_DLL_IMPORT
#define EP_LIBRARY_DLL_IMPORT
#endif //EP_LIBRARY_DLL_IMPORT

#else   //EP_SERVER_ENGINE_DLL_IMPORT

#define EP_SERVER_ENGINE

#endif  //EP_SERVER_ENGINE_DLL_IMPORT
#endif  //EP_SERVER_ENGINE_DLL_EXPORT

#include "epl.h"
using namespace epl;

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _CRTDBG_MAP_ALLOC
#endif //defined(_DEBUG)

#endif //__EP_SERVER_ENGINE_H__