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

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <cassert>
#include <cerrno>
#include "datahelper.h"
#include "logger.h"
#include "messages.h"
#include "artnet.h"

enum ArtNetOpcodes {
	OpPoll = 0x2000,
	OpPollReply = 0x2100,
	OpDiagData = 0x2300,
	OpCommand = 0x2400,
	OpDmx = 0x5000,
	OpNzs = 0x5100,
	OpAddress = 0x6000,
	OpInput = 0x7000,
	OpTodRequest = 0x8000,
	OpTodData = 0x8100,
	OpTodControl = 0x8200,
	OpRdm = 0x8300,
	OpRdmSub = 0x8400,
	OpVideoSetup = 0xa010,
	OpVideoPalette = 0xa020,
	OpVideoData = 0xa040,
	OpMacMaster = 0xf000,
	OpMacSlave = 0xf100,
	OpFirmwareMaser = 0xf200,
	OpFirmwareReply = 0xf300,
	OpFileTnMaster = 0xf400,
	OpFileFnMaster = 0xf500, 
	OpFileFnReply = 0xf600,
	OpIpProg = 0xf800,
	OpIpProgReply = 0xf900,
	OpMedia = 0x9000,
	OpMediaPatch = 0x9100,
	OpMediaControl = 0x9200,
	OpMediaContrlReply = 0x9300,
	OpTimeCode = 0x9700,
	OpTimeSync = 0x9800,
	OpTrigger = 0x9900,
	OpDirectory = 0x9a00,
	OpDirectoryReply = 0x9b00
};

ArtNet::ArtNet(std::string ip, IPC *ipc)
{
	assert(ipc != NULL);
	mIpc = ipc;
	
	// Create the socket
	mSocket = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
	if(mSocket < 0)
	{
		Error("ART-NET: Could not create socket: %s", strerror(errno));
		return;
	}
	
	// Set socket options
	int broadcast = 1;
	setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
	int reuse = 1;
	setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
	
	
	// Bind the socket
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(ARTNETPORT);
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(mSocket, (sockaddr *)&address, sizeof(address)) < 0)
	{
		Error("ART-NET: Could not bind the socket: %s", strerror(errno));
		return;
	}
	
	// Creating the buffer
	mBuffer = NULL;
	mBuffer = new char[ARTNETBUFFERSIZE];
	
	// Set the destination ip
	struct in_addr addr;
	inet_aton(ip.c_str(), &addr);
	mDest = addr.s_addr;
}

ArtNet::~ArtNet()
{
	if(mBuffer)
		delete[] mBuffer;
	if(mSocket > 0)
	{
		close(mSocket);
	}
}

bool ArtNet::IsValid() const
{
	return mSocket >0;
}

void ArtNet::Tick()
{
	assert(IsValid());
	struct sockaddr_in other;
	socklen_t otherLen;
	
	int flags = 0;
	ssize_t len;
	do {
		len = recvfrom(mSocket, mBuffer, ARTNETBUFFERSIZE-1, 0, (sockaddr *)&other, &otherLen);
		if(len < 0 && errno != EAGAIN)
		{
			Error("ART-NET: Error while reading socket: %s", strerror(errno));
			break;
		}
		if(len > 0 && IsArtNetMessage(mBuffer))
		{
			unsigned short opcode;
			unsigned short protVer;
			GetArtNetMessageInfo(mBuffer, &opcode, &protVer);
			//Debug("ART-NET: Received Art-Net Message len: %d, opcode: %04x, protVerHi: %02x, protVerLo: %02x", len, opcode, protVerHi, protVerLo);
			switch((ArtNetOpcodes)opcode)
			{
				case OpPoll:
					HandleOpPoll(mBuffer);
					break;
				case OpPollReply:
					HandleOpPollReply(mBuffer);
					break;
				case OpDmx:
					HandleOpDmx(mBuffer);
					break;
				
			}
			
		}
	} while(len > 0);
}

bool ArtNet::IsArtNetMessage(const char *buffer) const
{
	static const char ArtNetId[] = "Art-Net";
	assert(buffer != NULL);
	return memcmp(buffer, ArtNetId, sizeof(ArtNetId)) == 0;
}


void ArtNet::GetArtNetMessageInfo(const char * buffer, unsigned short *opcode,
	unsigned short *protVer) const
{
	assert(buffer != NULL);
	if(opcode)
		*opcode = DataHelper::GetUint16((const unsigned char *)buffer+8);
	if(protVer)
	{
		unsigned char protVerHi = DataHelper::GetUint8((const unsigned char *)buffer+10);
		unsigned char protVerLo = DataHelper::GetUint8((const unsigned char *)buffer+11);
		*protVer = (protVerHi<<8)|protVerLo;
	}
}

void ArtNet::HandleOpPoll(const char *buffer)
{
	// Send Poll reply
	static unsigned char reply[] = {
		'A', 'r', 't', '-', 'N', 'e', 't', 0, // ID
		(OpPollReply & 0xFF), (OpPollReply>>8)&0xFF, // Opcode
		192,168,2,100, // IP address
		(ARTNETPORT & 0xFF), (ARTNETPORT>>8)&0xFF, // Port
		0,0, // Version info
		(unsigned char)mNet, (unsigned char)mUniverse, // Universe
		0xFF, 0x7F, // OEM code prototypeing use
		0, // Ubea version
		0, // Status 1
		'R', 'J', // ESTA code
		'R', 'P', 'I', '-', 'S', 'P', 'I', ' ', 'D', 'M', 'X', 0,0,0,0,0,0,0, // Short name
		'R', 'P', 'I', '-', 'S', 'P', 'I', ' ', 'D', 'M', 'X', ' ', // Long name
		'h', 't', 't', 'p', ':', '/', '/', 'w', 'w', 'w', '.', 'r',
		'o', 'b', 'o', 'j', 'a', 'n', '.', 'n', 'l', 0, 0, 0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, // Node Report
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0, 1, // Input/output ports
		0x80, 0, 0, 0, // Port types
		0, 0, 0, 0, // Good input
		0, 0, 0, 0, // Good output
		0, 0, 0, 0, // swin
		0, 0, 0, 0, // swout
		0, // swvideo
		0, // swmacro
		0, // swRemote
		0, 0, 0, 0, // spare
		0, // style		
		0,0,0,0,0,0, // Mac address
		0,0,0,0, // Bind IP
		0, //Bound index
		0x02, // Status 2
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 // Filler
	};
	
	//Debug("ART-NET: Poll");
	
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(ARTNETPORT);
	address.sin_addr.s_addr = mDest;
	if(sendto(mSocket, reply, sizeof(reply), 0, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		Error("ART-NET: Could not send PollReply");
	}
}

void ArtNet::HandleOpPollReply(const char *buffer)
{
	//Debug("ART-NET: PollReply");
}

void ArtNet::HandleOpDmx(const char *buffer)
{
	unsigned char sequence = DataHelper::GetUint8((const unsigned char *)buffer+12);
	unsigned char physical = DataHelper::GetUint8((const unsigned char *)buffer+13);
	unsigned char subUni = DataHelper::GetUint8((const unsigned char *)buffer+14);
	unsigned char net = DataHelper::GetUint8((const unsigned char *)buffer+15);
	unsigned char lengthHi= DataHelper::GetUint8((const unsigned char *)buffer+16);
	unsigned char lengthLo= DataHelper::GetUint8((const unsigned char *)buffer+17);
	unsigned short length = (lengthHi<<8)|(lengthLo);
	
	IPCMessage *message = SetChannelsMessage(1, length, (const unsigned char *)buffer + 18);
	mIpc->SendMessage(*message);
	delete message;
	
	unsigned char ch1= DataHelper::GetUint8((const unsigned char *)buffer+18);
	
	//Debug("ART-NET: dmx Ch1: %d", ch1);
	/*Debug("ART-NET: Dmx packet: seq: %d, phy: %d, sub: %d, net: %d, length: %d",
		sequence, physical, subUni, net, length);*/
}