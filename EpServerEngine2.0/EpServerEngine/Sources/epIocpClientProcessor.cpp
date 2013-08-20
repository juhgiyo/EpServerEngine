/*! 
IocpClientProcessor for the EpServerEngine

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