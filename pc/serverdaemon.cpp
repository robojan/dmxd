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

#define USELIBARTNET


#include <unistd.h>
#include <stdint.h>
#include <cerrno> 
#include "global.h"
#include "logger.h"
#include "childs.h"
#include "ipc.h"
#include "datahelper.h"
#include "messages.h"
#include "settings.h"

#ifdef USELIBARTNET
#include <artnet/artnet.h>
#else
#include "artnet.h"
#endif

int dmx_handler(artnet_node n, int prt, void *d)
{
	IPC *ipc = (IPC *)d;
	
	
	int len;
	uint8_t *data;
	
	if(prt == 0) {
		data = artnet_read_dmx(n, prt, &len);
		IPCMessage *message = SetChannelsMessage(1, len, data);
		ipc->SendMessage(*message);
		delete message;
	}
	
	return 0;
}

int serverChild(int readfd, int writefd)
{
	IPC *ipc;
	// Initialize IPC
	ipc = new IPC(readfd, writefd);
	
	// Initialize artnet
#ifdef USELIBARTNET
	artnet_node node = artnet_new(Settings::GetArtNetIp().c_str(), 0);
	
	if(node == NULL) {
		Error("Could not create Art-Net node");
		return EXIT_COULD_NOT_START_SERVER;
	}
	
	artnet_set_short_name(node, Settings::GetArtNetShortName().c_str());
	artnet_set_long_name(node, Settings::GetArtNetLongName().c_str());
	artnet_set_node_type(node, ARTNET_NODE);
	artnet_set_port_type(node, 0, ARTNET_ENABLE_OUTPUT, ARTNET_PORT_DMX);
	artnet_set_port_addr(node, 0, ARTNET_OUTPUT_PORT, 0);
	//artnet_set_subnet_addr(node, 0);
	
	if(artnet_set_dmx_handler(node, dmx_handler, ipc)) {
		Error("Failed to install handler");
		return EXIT_COULD_NOT_START_SERVER;
	}
	
	artnet_start(node);
	
#else
	ArtNet artnet(Settings::GetArtNetIp().c_str(), ipc);
	if(!artnet.IsValid())
		return EXIT_COULD_NOT_START_SERVER;
#endif
	
	int channelCount = 0;
	
	int i = 1;
	// Main loop
	while(running)
	{
		// Handle IPC
		ipc->Tick();
		while(ipc->GetAvailableMessages() > 0)
		{
			IPCMessage *message;
			if((message = ipc->GetMessage()) != NULL)
			{
				//Debug("Received message: %s", message->ToString().c_str());
				
				switch(message->GetType())
				{
				case MSG_GETINFORESPONSE:
					channelCount = DataHelper::GetInt32((unsigned char *)message->GetData());
					break;
				case MSG_GETCHANNELSRESPONSE:
					break;
				}
				delete message;
			}
		}
		
		// Handle Artnet
#ifdef USELIBARTNET
		// This is blocking
		artnet_read(node, 0);
#else
		artnet.Tick();
#endif
		usleep(1000*15); // sleep for 15ms
	}
	
#ifdef USELIBARTNET
		// This is blocking
	artnet_stop(node);
	artnet_destroy(node);
#else
#endif
	
	return EXIT_OK;
}

