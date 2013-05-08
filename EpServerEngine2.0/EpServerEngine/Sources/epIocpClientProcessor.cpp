/*! 
IocpClientProcessor for the EpServerEngine
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
#include "epIocpClientProcessor.h"
#include "epIocpClientJob.h"
#include "epPacket.h"
#include "epBaseClient.h"
using namespace epse;

void IocpClientProcessor::DoJob(BaseWorkerThread *workerThread,  BaseJob* const data)
{
	IocpClientJob * job=reinterpret_cast<IocpClientJob*>(data);
	Packet *receivedPacket=NULL;
	SendStatus sendStatus;
	ReceiveStatus receiveStatus;
	switch(job->GetJobType())
	{
	case IocpClientJob::IOCP_CLIENT_JOB_TYPE_NULL:
		break;
	case IocpClientJob::IOCP_CLIENT_JOB_TYPE_SEND:
		job->GetClient()->Send(*job->GetPacket(),0,&sendStatus);
		
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
				job->GetCallBackObject()->OnSent(job->GetClient(),sendStatus);
			}
			else
				job->GetClient()->GetCallbackObject()->OnSent(job->GetClient(),sendStatus);
		}
		break;
	case IocpClientJob::IOCP_CLIENT_JOB_TYPE_RECEIVE:
		receivedPacket=job->GetClient()->Receive(0,&receiveStatus);
		
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
				job->GetCallBackObject()->OnReceived(job->GetClient(),receivedPacket,receiveStatus);
			}
			else
				job->GetClient()->GetCallbackObject()->OnReceived(job->GetClient(),receivedPacket,receiveStatus);
			
			if(receivedPacket)
				receivedPacket->ReleaseObj();
		}
		break;
	}
}

void IocpClientProcessor::handleReport(const JobProcessorStatus status)
{

}