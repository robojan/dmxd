/*The MIT License (MIT)

Copyright (c) 2015 Robbert-Jan de Jager

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
THE SOFTWARE.*/

#include <unistd.h>
#include <stdint.h>
#include "logger.h"
#include "settings.h"
#include "global.h"
#include "ipc.h"
#include "childs.h"
#include "datahelper.h"
#include "dmxcontrol.h"
#include "messages.h"
#include "stringhelper.h"

using namespace std;

int dmxChild(int readfd, int writefd)
{
	
	// Initialize IPC
	IPC *ipc = new IPC(readfd, writefd);
	
	// Initialize Dmx Control
	DmxControl control;
	if(!control.Open())
	{
		return EXIT_COULD_NOT_START_SPI;
	}
	
	if(ToLower(Settings::GetInitialState()) == "on")
	{
		control.SetAll(255);
	} else {
		control.SetAll(0);
	} 
	
	while(running)
	{
		ipc->Tick();
		while(ipc->GetAvailableMessages() > 0)
		{
			IPCMessage *message;
			if((message = ipc->GetMessage()) != NULL)
			{
				IPCMessage *response;
				uint8_t *values;
				//Debug("Received message: %s", message->ToString().c_str());
				switch(message->GetType())
				{
				case MSG_GETINFO:
					response = GetInfoResponse(control.GetChannelCount());
					ipc->SendMessage(*response);
					delete response;
					break;
				case MSG_GETCHANNELS:
					values = new uint8_t[control.GetChannelCount()];
					control.GetChannels(1, values, control.GetChannelCount());
					response = GetChannelsResponse(control.GetChannelCount(), values);
					ipc->SendMessage(*response);
					delete response;
					break;
				case MSG_SETCHANNELS:
					control.SetChannels(DataHelper::GetInt32((unsigned char *)message->GetData()), 
						(unsigned char *)message->GetData() + 4, 
						message->GetDataSize()-4);
					break;
				case MSG_SETCHANNEL:
					control.SetChannel(DataHelper::GetInt32((unsigned char *)message->GetData()),
						DataHelper::GetUint8((unsigned char *)message->GetData()+4));
					break;
				case MSG_SETALL:
					control.SetAll(DataHelper::GetInt8((unsigned char *)message->GetData()));
					break;
				}
				delete message;
			}
		}
		usleep(1000*15); // sleep for 15ms
	}
	
	if(ipc != NULL)
		delete ipc;
	
	return EXIT_OK;
	
}