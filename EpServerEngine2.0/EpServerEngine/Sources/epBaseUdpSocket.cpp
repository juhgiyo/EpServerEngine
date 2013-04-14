/*! 
BaseUdpSocket for the EpServerEngine
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
#include "epBaseUdpSocket.h"
#include "epBaseUdpServer.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;
BaseUdpSocket::BaseUdpSocket(ServerCallbackInterface *callBackObj,unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType): BaseSocket(callBackObj,waitTimeMilliSec,lockPolicyType)
{
	m_threadStopEvent=EventEx(false,false);
	switch(lockPolicyType)
	{
	case epl::LOCK_POLICY_CRITICALSECTION:
		m_listLock=EP_NEW epl::CriticalSectionEx();
		break;
	case epl::LOCK_POLICY_MUTEX:
		m_listLock=EP_NEW epl::Mutex();
		break;
	case epl::LOCK_POLICY_NONE:
		m_listLock=EP_NEW epl::NoLock();
		break;
	default:
		m_listLock=NULL;
		break;
	}
	m_maxPacketSize=0;

}

BaseUdpSocket::~BaseUdpSocket()
{
	m_listLock->Lock();

	Packet *removeElem=NULL;
	while(!m_packetList.empty())
	{
		removeElem=m_packetList.front();
		if(removeElem)
			removeElem->ReleaseObj();
		m_packetList.pop();
	}
	m_listLock->Unlock();

	if(m_listLock)
		EP_DELETE m_listLock;
	m_listLock=NULL;
}

void BaseUdpSocket::addPacket(Packet *packet)
{
	if(packet)
		packet->RetainObj();
	epl::LockObj lock(m_listLock);
	m_packetList.push(packet);
	if(GetStatus()==THREAD_STATUS_SUSPENDED)
		Resume();
}
void BaseUdpSocket::setMaxPacketByteSize(unsigned int maxPacketSize)
{
	m_maxPacketSize=maxPacketSize;
}

int BaseUdpSocket::Send(const Packet &packet, unsigned int waitTimeInMilliSec)
{
	epl::LockObj lock(m_baseSocketLock);
	EP_ASSERT(packet.GetPacketByteSize()<=m_maxPacketSize);
	if(m_owner)
		return ((BaseUdpServer*)m_owner)->send(packet,m_sockAddr,waitTimeInMilliSec);
	return 0;
}

unsigned int BaseUdpSocket::GetMaxPacketByteSize() const
{
	return m_maxPacketSize;
}




