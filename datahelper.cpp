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

#include "datahelper.h"


uint64_t DataHelper::GetUint64(const unsigned char *data)
{
	const uint8_t *ptr = (uint8_t *)data;
	uint64_t result = *ptr++;
	result |= ((uint64_t)(*ptr++))<<8;
	result |= ((uint64_t)(*ptr++))<<16;
	result |= ((uint64_t)(*ptr++))<<24;
	result |= ((uint64_t)(*ptr++))<<32;
	result |= ((uint64_t)(*ptr++))<<40;
	result |= ((uint64_t)(*ptr++))<<48;
	result |= ((uint64_t)(*ptr++))<<56;
	return result;
}

uint32_t DataHelper::GetUint32(const unsigned char *data)
{
	const uint8_t *ptr = (uint8_t *)data;
	uint32_t result = *ptr++;
	result |= (*ptr++)<<8;
	result |= (*ptr++)<<16;
	result |= (*ptr++)<<24;
	return result;
}

uint16_t DataHelper::GetUint16(const unsigned char *data)
{
	const uint8_t *ptr = (uint8_t *)data;
	uint16_t result = *ptr++;
	result |= (*ptr++)<<8;
	return result;
}

uint8_t DataHelper::GetUint8(const unsigned char *data)
{
	const uint8_t *ptr = (uint8_t *)data;
	uint8_t result = *ptr++;
	return result;
}

int64_t DataHelper::GetInt64(const unsigned char *data)
{
	uint64_t uresult = GetUint64(data);
	// TODO: implement c standard complient way
	int64_t result = (int64_t)uresult;
	return result;
}

int32_t DataHelper::GetInt32(const unsigned char *data)
{
	uint32_t uresult = GetUint32(data);
	// TODO: implement c standard complient way
	int32_t result = (int32_t)uresult;
	return result;
}

int16_t DataHelper::GetInt16(const unsigned char *data)
{
	uint16_t uresult = GetUint16(data);
	// TODO: implement c standard complient way
	int16_t result = (int16_t)uresult;
	return result;
}

int8_t DataHelper::GetInt8(const unsigned char *data)
{
	uint8_t uresult = GetUint8(data);
	// TODO: implement c standard complient way
	int8_t result = (int8_t)uresult;
	return result;
}

void DataHelper::SetUint64(unsigned char *data, uint64_t val)
{
	*data++ = (val>>0)&0xFF;
	*data++ = (val>>8)&0xFF;
	*data++ = (val>>16)&0xFF;
	*data++ = (val>>24)&0xFF;
	*data++ = (val>>32)&0xFF;
	*data++ = (val>>40)&0xFF;
	*data++ = (val>>48)&0xFF;
	*data++ = (val>>56)&0xFF;
}

void DataHelper::SetUint32(unsigned char *data, uint32_t val)
{
	*data++ = (val>>0)&0xFF;
	*data++ = (val>>8)&0xFF;
	*data++ = (val>>16)&0xFF;
	*data++ = (val>>24)&0xFF;
}
void DataHelper::SetUint16(unsigned char *data, uint16_t val)
{
	*data++ = (val>>0)&0xFF;
	*data++ = (val>>8)&0xFF;
}
void DataHelper::SetUint8(unsigned char *data, uint8_t val)
{
	*data++ = (val>>0)&0xFF;
}
void DataHelper::SetInt64(unsigned char *data, int64_t val)
{
	*data++ = (val>>0)&0xFF;
	*data++ = (val>>8)&0xFF;
	*data++ = (val>>16)&0xFF;
	*data++ = (val>>24)&0xFF;
	*data++ = (val>>32)&0xFF;
	*data++ = (val>>40)&0xFF;
	*data++ = (val>>48)&0xFF;
	*data++ = (val>>56)&0xFF;
}
void DataHelper::SetInt32(unsigned char *data, int32_t val)
{
	*data++ = (val>>0)&0xFF;
	*data++ = (val>>8)&0xFF;
	*data++ = (val>>16)&0xFF;
	*data++ = (val>>24)&0xFF;
}
void DataHelper::SetInt16(unsigned char *data, int16_t val)
{
	*data++ = (val>>0)&0xFF;
	*data++ = (val>>8)&0xFF;
}
void DataHelper::SetInt8(unsigned char *data, int8_t val)
{
	*data++ = (val>>0)&0xFF;
}