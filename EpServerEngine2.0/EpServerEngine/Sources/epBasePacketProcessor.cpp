/*! 
BasePacketProcessor for the EpServerEngine

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
*/
#include "epBasePacketProcessor.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

BasePacketProcessor::BasePacketProcessor(unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType):BaseServerObject(waitTimeMilliSec,lockPolicyType)
{
	m_owner=NULL;
	m_packetReceived=NULL;
}


BasePacketProcessor::~BasePacketProcessor()
{
	if(GetStatus()!=Thread::THREAD_STATUS_TERMINATED)
	{
		if(GetStatus()==Thread::THREAD_STATUS_SUSPENDED)
			Resume();
		TerminateAfter(m_waitTime);
		removeSelfFromContainer();
	}

	if(m_owner)
		m_owner->ReleaseObj();
	if(m_packetReceived)
		m_packetReceived->ReleaseObj();
	m_owner=NULL;
	m_packetReceived=NULL;
}

void BasePacketProcessor::setPacketPassUnit(const PacketPassUnit& packetPassUnit)
{
	if(m_packetReceived)
		m_packetReceived->ReleaseObj();
	m_packetReceived=packetPassUnit.m_packet;
	if(m_packetReceived)
		m_packetReceived->RetainObj();

	if(m_owner)
		m_owner->ReleaseObj();
	m_owner=packetPassUnit.m_owner;
	if(m_owner)
		m_owner->RetainObj();
}