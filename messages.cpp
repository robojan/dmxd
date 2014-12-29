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

#include "messages.h"
#include "datahelper.h"
#include <cstring>
#include <cstdint>

IPCMessage *GetInfoMessage()
{
	return new IPCMessage(MSG_GETINFO, 0, NULL);
}

IPCMessage *GetInfoResponse(int channels)
{
	uint8_t buffer[4];
	DataHelper::SetInt32(buffer, channels);
	return new IPCMessage(MSG_GETINFORESPONSE, sizeof(buffer), buffer);
}

IPCMessage *GetChannelsMessage()
{
	return new IPCMessage(MSG_GETCHANNELS, 0, NULL);
}

IPCMessage *GetChannelsResponse(int count, uint8_t *values)
{
	return new IPCMessage(MSG_GETCHANNELSRESPONSE, count, values);
}

IPCMessage *SetChannelsMessage(int address, int count, const uint8_t *values)
{
	uint8_t *buffer = new uint8_t[count+4];
	DataHelper::SetInt32(buffer, address);
	memcpy(buffer+4, values, count);
	IPCMessage *message = new IPCMessage(MSG_SETCHANNELS, count+4, buffer);
	delete[] buffer;
	return message;
}

IPCMessage *SetChannelMessage(int address, uint8_t value)
{
	uint8_t buffer[5];
	DataHelper::SetInt32(buffer, address);
	DataHelper::SetUint8(buffer+4, value);
	return new IPCMessage(MSG_SETCHANNEL, sizeof(buffer), buffer);
}

IPCMessage *SetAllMessage(uint8_t value)
{
	return new IPCMessage(MSG_SETALL, sizeof(uint8_t), &value);
} 