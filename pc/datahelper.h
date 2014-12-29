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
#ifndef _DATAHELPER_H_
#define _DATAHELPER_H_

#include <stdint.h>

class DataHelper {
public:
	static uint64_t GetUint64(const unsigned char *data);
	static uint32_t GetUint32(const unsigned char *data);
	static uint16_t GetUint16(const unsigned char *data);
	static uint8_t GetUint8(const unsigned char *data);
	static int64_t GetInt64(const unsigned char *data);
	static int32_t GetInt32(const unsigned char *data);
	static int16_t GetInt16(const unsigned char *data);
	static int8_t GetInt8(const unsigned char *data);
	static void SetUint64(unsigned char *data, uint64_t val);
	static void SetUint32(unsigned char *data, uint32_t val);
	static void SetUint16(unsigned char *data, uint16_t val);
	static void SetUint8(unsigned char *data, uint8_t val);
	static void SetInt64(unsigned char *data, int64_t val);
	static void SetInt32(unsigned char *data, int32_t val);
	static void SetInt16(unsigned char *data, int16_t val);
	static void SetInt8(unsigned char *data, int8_t val);
private:

};

#endif