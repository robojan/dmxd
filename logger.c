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
#include <stdio.h>
#include <syslog.h>
#include <stdarg.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "logger.h"

void syslogLoggerImpl(enum LogLevel level, const char *format, va_list arg);
void stdoutLoggerImpl(enum LogLevel level, const char *format, va_list arg);

// Simple linked list nodes
struct LoggerNode {
	LogFunc_t logger;
	struct LoggerNode *next;
};

const LogFunc_t syslogLogger = syslogLoggerImpl;
const LogFunc_t stdoutLogger = stdoutLoggerImpl;

// Default logger is stdout
struct LoggerNode defaultLogger = {stdoutLoggerImpl, NULL};

// Create the root of the linked list
struct LoggerNode *rootLogger = &defaultLogger;

// Send the log message to syslog
void syslogLoggerImpl(enum LogLevel level, const char *format, va_list arg)
{
	assert(format != NULL);
	int priority;
	switch(level)
	{
		case LOGDEBUG:
			priority = LOG_DEBUG;
			break;
		case LOGINFORM:
			priority = LOG_INFO;
			break;
		case LOGNOTICE:
			priority = LOG_NOTICE;
			break;
		case LOGWARN:
			priority = LOG_WARNING;
			break;
		case LOGERROR:
			priority = LOG_ERR;
			break;
		case LOGCRITICAL:
			priority = LOG_CRIT;
			break;
		default:
			priority = LOG_INFO;
			break;
	}
	vsyslog(priority, format, arg);
}

// Send the log message to stdout/stderr
void stdoutLoggerImpl(enum LogLevel level, const char *format, va_list arg)
{
	assert(format != NULL);
	char *extendedFormat = (char *) malloc(strlen(format)+15);
	switch(level)
	{
		case LOGDEBUG:
			strcpy(extendedFormat, "DEBUG: ");
			break;
		case LOGINFORM:
			strcpy(extendedFormat, "INFORM: ");
			break;
		case LOGNOTICE:
			strcpy(extendedFormat, "NOTICE: ");
			break;
		case LOGWARN:
			strcpy(extendedFormat, "WARN: ");
			break;
		case LOGERROR:
			strcpy(extendedFormat, "ERROR: ");
			break;
		case LOGCRITICAL:
			strcpy(extendedFormat, "CRITICAL: ");
			break;
		default:
			strcpy(extendedFormat, "");
			break;
	}
	strcat(extendedFormat, format);
	strcat(extendedFormat, "\n");
	vfprintf(stdout, extendedFormat, arg);
	free(extendedFormat);
}

// Send the log message to all registered loggers
void vLogger(enum LogLevel level, const char *format, va_list arg)
{
	struct LoggerNode *node = rootLogger;
	while(node != NULL)
	{
		node->logger(level, format, arg);
		node = node->next;
	}
}

// Remove Node when there is no node after it
void ClearLogger(struct LoggerNode *node)
{
	// recursive function
	if(node == NULL)
		return;
	
	ClearLogger(node->next);
	// The defaultLogger isn't dynamically created
	if(node != &defaultLogger)
	{
		free(node);
	}
}

// Remove all the loggers
void ClearLoggers()
{
	ClearLogger(rootLogger);
	rootLogger = NULL;
}

void AddLoggerToList(const LogFunc_t logger, struct LoggerNode *node)
{
	assert(node != NULL);
	
	// When the end of the list is reached create a new node
	if(node->next == NULL)
	{
		struct LoggerNode *newNode = (struct LoggerNode *) malloc(sizeof(struct LoggerNode));
		newNode->next = NULL;
		newNode->logger = logger;
		node->next = newNode;
	} else {
		// Go to the next node
		AddLoggerToList(logger, node->next);
	}
}

// Add an logger
void AddLogger(const LogFunc_t logger)
{
	// When the logger is the default logger replace it by a dynamically allocated version
	if(rootLogger == &defaultLogger)
	{
		rootLogger = NULL;
		rootLogger = (struct LoggerNode *) malloc(sizeof(struct LoggerNode));
		rootLogger->logger = stdoutLogger;
		rootLogger->next = NULL;
	}
	// When the list is empty set the logger as the root
	// Else add the logger to the list
	if(rootLogger == NULL)
	{
		struct LoggerNode *newNode = (struct LoggerNode *) malloc(sizeof(struct LoggerNode));
		newNode->next = NULL;
		newNode->logger = logger;
		rootLogger = newNode;
	} else {
		AddLoggerToList(logger, rootLogger);
	}
}

// Remove an logger from the list
void RemoveLoggerFromList(const LogFunc_t logger, struct LoggerNode **node)
{
	if(*node == NULL)
		return;
	if((*node)->logger == logger)
	{
		struct LoggerNode *next = (*node)->next;
		if(*node != &defaultLogger)
		{
			free(*node);
		}
		*node = next;		
	} else {
		RemoveLoggerFromList(logger, &((*node)->next));
	}
}

// Remove an logger
void RemoveLogger(const LogFunc_t logger)
{
	RemoveLoggerFromList(logger, &rootLogger);
}


// Wrappers for loggers
void Debug(const char *format, ...)
{
	va_list a_list;
	va_start(a_list, format);
	vLogger(LOGDEBUG, format, a_list);
	va_end(a_list);
}

void Inform(const char *format, ...)
{
	va_list a_list;
	va_start(a_list, format);
	vLogger(LOGINFORM, format, a_list);
	va_end(a_list);
}

void Notice(const char *format, ...)
{
	va_list a_list;
	va_start(a_list, format);
	vLogger(LOGNOTICE, format, a_list);
	va_end(a_list);
}

void Warn(const char *format, ...)
{
	va_list a_list;
	va_start(a_list, format);
	vLogger(LOGWARN, format, a_list);
	va_end(a_list);
}

void Error(const char *format, ...)
{
	va_list a_list;
	va_start(a_list, format);
	vLogger(LOGERROR, format, a_list);
	va_end(a_list);
}

void Critical(const char *format, ...)
{
	va_list a_list;
	va_start(a_list, format);
	vLogger(LOGCRITICAL, format, a_list);
	va_end(a_list);
}