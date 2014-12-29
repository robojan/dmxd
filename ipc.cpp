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
#include <cerrno>
#include <cstring>
#include <cassert>
#include <sstream>
#include <fcntl.h>
#include "logger.h"
#include "ipc.h"
#include "datahelper.h"

IPCMessage::IPCMessage(const unsigned char *data)
{
	mType = DataHelper::GetInt32(data);
	mSize = DataHelper::GetInt32(data+4);
	if(mSize == 0)
	{
		mData = NULL;
	} else {
		mData = malloc(mSize);
		memcpy(mData, data+8, mSize);
	}
}


// Empty constructor
IPCMessage::IPCMessage()
{
	mType = -1;
	mSize = 0;
	mData = NULL;
}

// Constructor for setting the data
IPCMessage::IPCMessage(int type, int size, const void *data)
{
	mType = type;
	mSize = size;
	if(mSize == 0)
	{
		mData = NULL;
	} else {
		mData = malloc(mSize);
		memcpy(mData, data, mSize);
	}
}

// Destructor for IPCMessage
IPCMessage::~IPCMessage()
{
	if(mData != NULL)
		free(mData);
}

// Function that returns the type of the message
int IPCMessage::GetType() const
{
	return mType;
}

// Get the data
const void *IPCMessage::GetData() const
{
	return mData;
}

// Get data size
int IPCMessage::GetDataSize() const
{
	return mSize;
}

// Function that returns an human readable representation of the mesage
std::string IPCMessage::ToString() const
{
	std::stringstream ss;
	ss << "Message{Type:" << mType << " Size:" << mSize << "}";
	return ss.str();
}

// Constructor
IPC::IPC(int readfd, int writefd)
{
	mReadFd = readfd;
	mWriteFd = writefd;
	mReadingBufferSize = 0;
	fcntl(mReadFd, F_SETFL, O_NONBLOCK);
	fcntl(mWriteFd, F_SETFL, O_NONBLOCK);
}

// Destructor
IPC::~IPC()
{
	
}


// This function reads data from the pipe and tries to create an message from it
// This function needs to be called periodically to prevent buffer overflows
void IPC::Tick()
{
	ssize_t len;
	// Read all available data into buffer
	do {
		len = read(mReadFd, mReadingBuffer+mReadingBufferSize, IPC_RECEIVINGBUFFERSIZE - mReadingBufferSize);
		if(len < 0)
		{
			if(errno != EAGAIN)
			{
				Error("Could not read IPC channel: %s", strerror(errno));
			}
			break;
		}
		mReadingBufferSize+=len;
	} while(len > 0);
	// Try to read messages
	while(mReadingBufferSize >= 8)
	{
		int type = DataHelper::GetInt32(mReadingBuffer);
		int size = DataHelper::GetInt32(mReadingBuffer+4);
		if(mReadingBufferSize < 8+size)
		{
			// Buffer not large enoug
			break;
		}
		IPCMessage *message = new IPCMessage(mReadingBuffer);
		mMessageQueue.push(message);
		memmove(mReadingBuffer, mReadingBuffer+size+8, mReadingBufferSize - size - 8);
		mReadingBufferSize -= size + 8;
	}
}

// Send an message to the other side
void IPC::SendMessage(const IPCMessage &message)
{
	unsigned char *buffer = new unsigned char[message.GetDataSize() + 8];
	DataHelper::SetInt32(buffer, message.GetType());
	DataHelper::SetInt32(buffer+4, message.GetDataSize());
	memcpy(buffer+8, message.GetData(), message.GetDataSize());
	size_t written = 0;
	while(written < message.GetDataSize() + 8)
	{
		ssize_t result = write(mWriteFd, buffer+written, message.GetDataSize() + 8 - written);
		if(result < 0)
		{
			if(errno != EAGAIN)
			{
				Error("Could not write IPC channel: %s", strerror(errno));
			}
			break;
		}
		written+=result;
	}
	delete[] buffer;
}

// This function returns an message in message which is received from the pipe
// Returns true when successful
IPCMessage *IPC::GetMessage()
{
	IPCMessage *message;
	if(mMessageQueue.size() == 0)
		return NULL;
	message = mMessageQueue.front();
	mMessageQueue.pop();
	return message;
}

// This function returns the number of received messages
int IPC::GetAvailableMessages()
{
	return mMessageQueue.size();
}