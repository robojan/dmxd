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
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/prctl.h>
#include <syslog.h>
#include <string.h>
#include "settings.h"
#include "childs.h"
#include "global.h"
#include "logger.h"

// global state variables
int running = 1;

void signal_handler(int sig, siginfo_t *siginfo, void *context)
{
	Debug("Signal received: %d", sig);
	switch(sig) {
		case SIGCHLD: // Child stopped or terminated
			running = 0;
			break;
		case SIGTERM: // Termination signal
			running = 0;
			break;
		default:
			Warn("Unknown signal received %d", sig);
			break;
	}
	
}

//
// Main function
//
int main(int argc, char **argv)
{
	// Add syslog logger
	openlog("DMXD", LOG_PID, LOG_DAEMON);
	AddLogger(syslogLogger);
	
	// Load the settings
	Settings::Load(argc, argv);
	
	// Check for root priveleges
	int currentUser = getuid();
	if(currentUser != 0)
	{
		Error("This application needs to be executed as root"); 
		return -1;
	}
	
	Inform("DMX daemon is starting");
	
	
	// start the daemon
	pid_t pid;
	
	// Fork off the parent process
	pid = fork();
	if(pid < 0) exit(EXIT_COULD_NOT_CREATE_CHILDS); // error
	if(pid > 0) exit(EXIT_OK); // success, terminate the parent
	
	// child process
	if(setsid() < 0)
		exit(EXIT_COULD_NOT_CREATE_CHILDS);
		
	// Handle the signals
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_sigaction = &signal_handler;
	act.sa_flags = SA_SIGINFO;

	sigaction(SIGCHLD, &act, NULL);
	sigaction(SIGTERM, &act, NULL);	
	signal(SIGHUP, SIG_IGN);
	
	// Fork second time
	if(pid < 0) exit(EXIT_COULD_NOT_CREATE_CHILDS); // error
	if(pid > 0) exit(EXIT_OK); // terminate the parent
	
	// Set new file permissions
	umask(0);
	
	// Change working directory
	chdir(WORKING_DIRECTORY);
	
	// Close all open file descriptors
	int fd;
	for(fd = sysconf(_SC_OPEN_MAX); fd> 0; fd--)
	{
		close(fd);
	}
	
	Debug("Creating the pipe");
	int pipes1[2];
	int pipes2[2];
	
	if(pipe(pipes1) < 0 || pipe(pipes2) < 0) {
		// Could not create pipe
		Error("Could not create pipe");
		exit(EXIT_COULD_NOT_CREATE_CHILDS);
	}
	
	// Disconnect all loggers
	closelog();
	ClearLoggers();
	
	// Create two childs one dmx server and one tcp server
	pid = fork();
	if(pid < 0) exit(EXIT_COULD_NOT_CREATE_CHILDS); // error
	
	int result;
	if(pid == 0) // child
	{
		// Becomes server daemon
		// Open the log file
		openlog("DMXD_server", LOG_PID, LOG_DAEMON);
		AddLogger(syslogLogger);
		Inform("Starting DMX Server daemon");

		sigaction(SIGTERM, &act, NULL);		
		
		// Change user
		int user = Settings::GetServerUser();
		setuid(user);
		
		// Start the child
		result = serverChild(pipes2[0], pipes1[1]);
		
		Inform("Stopping DMX Server daemon");
		
	} else {
		// Becomes dmx daemon
		// Open the log file
		openlog("DMXD_dmx", LOG_PID, LOG_DAEMON);
		AddLogger(syslogLogger);
		Inform("Starting DMX daemon");

			
		// sent SIGHUP signal when parent dies
		prctl(PR_SET_PDEATHSIG, SIGHUP);	
		
		// Change user
		int user = Settings::GetDMXUser();
		setuid(user);
		
		// Start the child
		result = dmxChild(pipes1[0], pipes2[1]);
		
		Inform("Stopping DMX daemon");
		
		// kill the child
		kill(pid, SIGTERM);
	}
	
	closelog();
	// Close all the pipes
	close(pipes1[0]);
	close(pipes1[1]);
	close(pipes2[0]);
	close(pipes2[1]);
	return result;
}
