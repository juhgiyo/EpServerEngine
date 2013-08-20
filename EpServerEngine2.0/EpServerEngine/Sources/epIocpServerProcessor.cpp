/*! 
IocpServerProcessor for the EpServerEngine

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
#include "epIocpServerProcessor.h"
#include "epIocpServerJob.h"
#include "epPacket.h"
using namespace epse;

void IocpServerProcessor::DoJob(BaseWorkerThread *workerThread,  BaseJob* const data)
{
	IocpServerJob * job=reinterpret_cast<IocpServerJob*>(data);
	Packet *receivedPacket=NULL;
	SendStatus sendStatus;
	ReceiveStatus receiveStatus;
	switch(job->GetJobType())
	{
	case IocpServerJob::IOCP_SERVER_JOB_TYPE_NULL:
		break;
	case IocpServerJob::IOCP_SERVER_JOB_TYPE_SEND:
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
	case IocpServerJob::IOCP_SERVER_JOB_TYPE_RECEIVE:
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
	case IocpServerJob::IOCP_SERVER_JOB_TYPE_DISCONNECT:
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

void IocpServerProcessor::handleReport(const JobProcessorStatus status)
{

}