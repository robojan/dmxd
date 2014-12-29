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
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#define CONFIG_FILE "dmxd.conf"
#define DEFAULT_PORTNUM 0
#define DEFAULT_PORTSPEED 10
#define DEFAULT_SERVERUSER "http"
#define DEFAULT_DMXUSER "root"
#define DEFAULT_ARTNETIP "0.0.0.0"
#define DEFAULT_ARTNETLONGNAME "SPI-DMX Art-Net daemon http://www.robojan.nl"
#define DEFAULT_ARTNETSHORTNAME "SPI-DMX daemon"
#define DEFAULT_INITIALSTATE "off"

#define WORKING_DIRECTORY "/"

enum exitCodes {
	EXIT_OK,
	EXIT_COULD_NOT_START_SPI,
	EXIT_COULD_NOT_CREATE_CHILDS,
	EXIT_COULD_NOT_START_SERVER,
};

#endif
