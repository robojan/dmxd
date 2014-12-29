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
#ifndef _LOGGER_H_
#define _LOGGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>


// Different log levels
enum LogLevel {
	LOGDEBUG,
	LOGINFORM,
	LOGNOTICE,
	LOGWARN,
	LOGERROR,
	LOGCRITICAL,
};

// Varius logging functions of different logging levels
void Debug(const char *format, ...);
void Inform(const char *format, ...);
void Notice(const char *format, ...);
void Warn(const char *format, ...);
void Error(const char *format, ...);
void Critical(const char *format, ...);

// General Logger function
void vLogger(enum LogLevel level, const char *format, va_list arg);

// Logging functions types
typedef void (*LogFunc_t)(enum LogLevel, const char *, va_list);

// Remove all the loggers
void ClearLoggers();

// Add an logger
void AddLogger(const LogFunc_t logger);

// Remove an logger
void RemoveLogger(const LogFunc_t logger);

// Some default loggers
extern const LogFunc_t syslogLogger;
extern const LogFunc_t stdoutLogger;


#ifdef __cplusplus
}
#endif

#endif