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
#ifndef _ARTNET_H_
#define _ARTNET_H_

#include <string>
#include "ipc.h"

#define ARTNETPORT	6454
#define ARTNETBUFFERSIZE 10000
#define ARTNETPROTVER 14

class ArtNet {
public:
	ArtNet(std::string ip, IPC *ipc);
	~ArtNet();
	
	bool IsValid() const;
	
	void Tick();
private:
	void HandleOpPoll(const char *buffer);
	void HandleOpPollReply(const char *buffer);
	void HandleOpDmx(const char *buffer);
	
	bool IsArtNetMessage(const char *buffer) const;
	void GetArtNetMessageInfo(const char * buffer, unsigned short *opcode,
		unsigned short *protVer) const;

	int mChannelCount;
	int mUniverse;
	int mNet;
	IPC *mIpc;
	unsigned int mDest;
	char *mBuffer;
	int mSocket;
};

#endif