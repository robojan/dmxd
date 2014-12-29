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

#include <wiringPiSPI.h>
#include <string.h>
#include <unistd.h>
#include "logger.h"
#include "settings.h"
#include "global.h"
#include "dmxcontrol.h"

DmxControl::DmxControl()
{

}

DmxControl::~DmxControl()
{
	Close();
	
}

int DmxControl::GetChannelCount()
{
	return mChannelCount;
}

void DmxControl::UpdateChannels()
{
	for(int i = 0; i< mChannelCount; i+=8)
	{
		// Get 8 channels from the interface
		uint8_t values[8];
		if(!GetDMXChannels(i + 1, values))
		{
			Error("Could not get current channel values from interface");
			continue;
		}
		// Write the values to the local buffer if it fits 
		for(int j = 0; j<8; j++)
		{
			if(i+j >= mChannelCount)
				break;
			mChannels[i+j] = values[j];
		}
	}
}

void DmxControl::RefreshChannels()
{
	for(int i = 0; i< mChannelCount; i+=8)
	{
		uint8_t values[8];
		// Write the values to the local buffer if it fits 
		for(int j = 0; j<8; j++)
		{
			if(i+j >= mChannelCount)
				break;
			values[j] = mChannels[i+j];
		}
		SetDMXChannels(i + 1, values);
	}
}

void DmxControl::SetAll(uint8_t val)
{
	for(int i = 0; i < mChannelCount; i++)
	{
		mChannels[i] = val;
	}
	RefreshChannels();
}

void DmxControl::SetChannel(int address, uint8_t value)
{
	mChannels[address-1] = value;
	SetDMXChannel(address, value, 255, 0, 255, 0, 255, 0);
}

int DmxControl::GetChannels(int address, uint8_t *values, int size)
{
	if(address + size > mChannelCount)
	{
		size = mChannelCount - address;
	}
	memcpy(values, mChannels+address-1, size);
	return size;
}

int DmxControl::SetChannels(int address, uint8_t *values, int size)
{
	if(address + size > mChannelCount)
	{
		size = mChannelCount - address;
	}
	memcpy(mChannels + address-1, values, size);
	RefreshChannels();
	return size;
}

bool DmxControl::Open()
{
	// Get the settings
	int port = Settings::GetSPIPort();
	int speed = Settings::GetSPISpeed();
	
	// Initialize the SPI library
	Debug("opening SPI port");
	mFd = wiringPiSPISetup(port, speed*1000);
	if(mFd == -1)
	{
		Error("Could not open SPI port %d with speed %dkHz: %s", port, speed, strerror(errno));
		return false;
	}
	
	// Get information from the interface
	if(!GetDMXInfo(&mChannelCount))
	{
		Error("Could not get number of supported channels from interface");
		return false;
	}
	Inform("Interface has %d channels", mChannelCount);
	
	// Get the current channel values
	mChannels = new uint8_t[mChannelCount];
	UpdateChannels();
	return true;
}

void DmxControl::Close()
{
	if(mFd != -1)
		close(mFd);
}

bool DmxControl::GetDMXInfo(int *channels)
{
//	Debug("DmxInfo");
	uint8_t buffer[10] = {0x04, 0x00}; // Get Info command
	wiringPiSPIDataRW(Settings::GetSPIPort(), buffer, sizeof(buffer)); // Send commando, result is garbage
//	Debug("SPIBuffer: %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x %02x %02x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9]);
	memset(buffer, 0, 10); // Ignore command
	wiringPiSPIDataRW(Settings::GetSPIPort(), buffer, sizeof(buffer)); // Send Ignore command returns the result
//	Debug("SPIBuffer: %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x %02x %02x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9]);
	if(buffer[0] == 4)
	{
		if(channels)
			*channels = buffer[1];
		return true;
	}
	return false;
}

bool DmxControl::GetDMXChannels(int address, uint8_t *channels)
{
	//Debug("Get DMX channels");
	uint8_t buffer[10] = {0x02, (uint8_t) address, 0}; // Get Info command
	wiringPiSPIDataRW(Settings::GetSPIPort(), buffer, sizeof(buffer)); // Send commando, result is garbage
	//Debug("SPIBuffer: %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x %02x %02x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9]);
	memset(buffer, 0, 10); // Ignore command
	wiringPiSPIDataRW(Settings::GetSPIPort(), buffer, sizeof(buffer)); // Send Ignore command returns the result
	//Debug("SPIBuffer: %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x %02x %02x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9]);
	if(buffer[0] == 2)
	{
		if(channels)
		{
			for(int i = 0; i<8; i++)
			{
				channels[i] = buffer[2+i];
			}
		}
		return true;
	}
	return false;
}

void DmxControl::SetDMXChannels(int address, uint8_t *channels)
{
	uint8_t buffer[10] = {0x03, (uint8_t) address, 0}; // Get Info command
	if(channels)
	{
		for(int i = 0; i<8; i++)
		{
			buffer[2+i] = channels[i];
		}
	}
	//Debug("SPIBuffer: %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x %02x %02x", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9]);
	wiringPiSPIDataRW(Settings::GetSPIPort(), buffer, sizeof(buffer)); // Send commando, result is garbage
}

void DmxControl::SetDMXChannel(int address1, uint8_t channel1, int address2, uint8_t channel2,int address3, uint8_t channel3,int address4, uint8_t channel4)
{
	uint8_t buffer[10] = {0x01, (uint8_t) address1, channel1, (uint8_t) address2, channel2, (uint8_t) address3, channel3, (uint8_t) address4, channel4, 0}; // Set Channel
	wiringPiSPIDataRW(Settings::GetSPIPort(), buffer, sizeof(buffer)); // Send commando, result is garbage
}