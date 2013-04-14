/*! 
BasePacketProcessor for the EpServerEngine
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