/*! 
SyncTcpSocket for the EpServerEngine
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
#include "epSyncTcpSocket.h"
#include "epSyncTcpServer.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;
SyncTcpSocket::SyncTcpSocket(ServerCallbackInterface *callBackObj,unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType): BaseTcpSocket(callBackObj,waitTimeMilliSec,lockPolicyType)
{
}

SyncTcpSocket::~SyncTcpSocket()
{
	KillConnection();
}



void SyncTcpSocket::KillConnection()
{
	epl::LockObj lock(m_baseSocketLock);
	if(!IsConnectionAlive())
	{
		return;
	}
	// No longer need client socket
	if(m_clientSocket!=INVALID_SOCKET)
	{
		int iResult;
		iResult = shutdown(m_clientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			epl::System::OutputDebugString(_T("%s::%s(%d)(%x) shutdown failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
		}
		closesocket(m_clientSocket);
		m_clientSocket = INVALID_SOCKET;
	}
	TerminateAfter(m_waitTime);
	removeSelfFromContainer();
	m_callBackObj->OnDisconnect(this);
}


void SyncTcpSocket::killConnection()
{
	if(IsConnectionAlive())
	{
		// No longer need client socket
		removeSelfFromContainer();
		m_callBackObj->OnDisconnect(this);
	}
}


void SyncTcpSocket::execute()
{
	int iResult=0;

	// Receive until the peer shuts down the connection
	do {
		iResult =receive(m_recvSizePacket);
		if(iResult>0)
		{
			unsigned int shouldReceive=(reinterpret_cast<unsigned int*>(const_cast<char*>(m_recvSizePacket.GetPacket())))[0];
			Packet *recvPacket=EP_NEW Packet(NULL,shouldReceive);
			iResult = receive(*recvPacket);

			if (iResult == shouldReceive) {
				m_callBackObj->OnReceived(this,&recvPacket);
				recvPacket->ReleaseObj();
			}
			else if (iResult == 0)
			{
				epl::System::OutputDebugString(_T("%s::%s(%d)(%x) Connection closing...\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
				recvPacket->ReleaseObj();
				break;
			}
			else  {
				epl::System::OutputDebugString(_T("%s::%s(%d)(%x) recv failed with error\r\n"),__TFILE__,__TFUNCTION__,__LINE__,this);
				recvPacket->ReleaseObj();
				break;
			}
		}
		else
		{
			break;
		}

	} while (iResult > 0);

	killConnection();
}


