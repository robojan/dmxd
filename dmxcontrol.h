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
#ifndef _DMXCONTROL_H_
#define _DMXCONTROL_H_

#include <stdint.h>

class DmxControl
{
public:
	DmxControl();
	~DmxControl();
	
	bool Open();
	void Close();
	
	int GetChannelCount();
	
	void UpdateChannels();
	void RefreshChannels();
	void SetAll(uint8_t val);
	void SetChannel(int address, uint8_t value);
	int GetChannels(int address, uint8_t *values, int size);
	int SetChannels(int address, uint8_t *values, int size);
	
	
private:
	bool GetDMXInfo(int *channels);
	bool GetDMXChannels(int address, uint8_t *channels);
	void SetDMXChannels(int address, uint8_t *channels);
	void SetDMXChannel(int address1, uint8_t channel1, int address2, uint8_t channel2,int address3, uint8_t channel3,int address4, uint8_t channel4);
	
	int mFd;
	int mChannelCount;
	uint8_t *mChannels;
};


#endif