/*! 
IocpTcpProcessor for the EpServerEngine
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
#include "epIocpTcpProcessor.h"
#include "epIocpTcpJob.h"
#include "epPacket.h"
using namespace epse;

void IocpTcpProcessor::DoJob(BaseWorkerThread *workerThread,  BaseJob* const data)
{
	IocpTcpJob * job=reinterpret_cast<IocpTcpJob*>(data);
	Packet *receivedPacket=NULL;
	SendStatus sendStatus;
	ReceiveStatus receiveStatus;
	switch(job->GetJobType())
	{
	case IocpTcpJob::IOCP_TCP_JOB_TYPE_NULL:
		break;
	case IocpTcpJob::IOCP_TCP_JOB_TYPE_SEND:
		job->GetSocket()->Send(*job->GetPacket(),0,&sendStatus);
		
		if(sendStatus==SEND_STATUS_FAIL_TIME_OUT)
		{
			workerThread->Push(data);
		}
		else
		{
			if(job->GetCompletionEvent())
				job->GetCompletionEvent()->SetEvent();
			if(job->GetCallBackObject())
			{
				job->GetCallBackObject()->OnSent(job->GetSocket(),sendStatus);
			}
			else
				job->GetSocket()->GetCallbackObject()->OnSent(job->GetSocket(),sendStatus);
		}
		break;
	case IocpTcpJob::IOCP_TCP_JOB_TYPE_RECEIVE:
		receivedPacket=job->GetSocket()->Receive(0,&receiveStatus);
		
		if(receiveStatus==RECEIVE_STATUS_FAIL_TIME_OUT)
		{
			workerThread->Push(data);
		}
		else
		{
			if(job->GetCompletionEvent())
				job->GetCompletionEvent()->SetEvent();
			if(job->GetCallBackObject())
			{
				job->GetCallBackObject()->OnReceived(job->GetSocket(),receivedPacket,receiveStatus);
			}
			else
				job->GetSocket()->GetCallbackObject()->OnReceived(job->GetSocket(),receivedPacket,receiveStatus);
			
			if(receivedPacket)
				receivedPacket->ReleaseObj();
		}
		break;
	case IocpTcpJob::IOCP_TCP_JOB_TYPE_DISCONNECT:
		job->GetSocket()->killConnectionNoCallBack();

		if(job->GetCompletionEvent())
			job->GetCompletionEvent()->SetEvent();
		if(job->GetCallBackObject())
		{
			job->GetCallBackObject()->OnDisconnect(job->GetSocket());
		}
		else
			job->GetSocket()->GetCallbackObject()->OnDisconnect(job->GetSocket());

		break;
	}
}

void IocpTcpProcessor::handleReport(const JobProcessorStatus status)
{

}