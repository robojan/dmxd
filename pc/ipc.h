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
#ifndef _IPC_H_
#define _IPC_H_

#include <string>
#include <queue>

#define IPC_RECEIVINGBUFFERSIZE 8192

// Protocol used for IPC:
// 4 bytes int type
// 4 bytes for length of data
// n bytes for data

// Inter process communication message
class IPCMessage 
{
public:
	// Constructor for IPCMessage
	// data is a pointer to the raw received data
	IPCMessage(const unsigned char *data);
	// Empty constructor
	IPCMessage();
	// Constructor for setting the data
	IPCMessage(int type, int size, const void *data);
	// Destructor for IPCMessage
	~IPCMessage();
	
	// Function that returns the type of the message
	int GetType() const;
	// Get the data
	const void *GetData() const;
	int GetDataSize() const;
	// Function that returns an human readable representation of the mesage
	std::string ToString() const;	
	
private:
	int mType;
	void *mData;
	int mSize;
};

// Inter process communication class 
class IPC 
{
public:
	// Constructor for the inter process communications class
	// readfd and writefd are file descriptors for the pipes used for IPC
	IPC(int readfd, int writefd);
	// Destructor for IPC
	~IPC();
	
	// This function reads data from the pipe and tries to create an message from it
	// This function needs to be called periodically to prevent buffer overflows
	void Tick();
	// Send an message to the other side
	void SendMessage(const IPCMessage &message);
	// This function returns an message in message which is received from the pipe
	// Returns true when successful
	IPCMessage *GetMessage();
	// This function returns the number of received messages
	int GetAvailableMessages();
private:
	int mReadFd;
	int mWriteFd;
	std::queue<IPCMessage *> mMessageQueue;
	int mReadingBufferSize;
	unsigned char mReadingBuffer[IPC_RECEIVINGBUFFERSIZE];
};

#endif