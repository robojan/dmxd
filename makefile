#The MIT License (MIT)
#
#Copyright (c) 2015 Robbert-Jan de Jager
#
#Permission is hereby granted, free of charge, to any person obtaining a copy
#of this software and associated documentation files (the "Software"), to deal
#in the Software without restriction, including without limitation the rights
#to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#copies of the Software, and to permit persons to whom the Software is
#furnished to do so, subject to the following conditions:

#The above copyright notice and this permission notice shall be included in
#all copies or substantial portions of the Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
#THE SOFTWARE.

# Project name: also outputfile name
OUTPUT=dmxd

# C Source files
CSRCS=logger.c
# C++ Source files
CPPSRCS=main.cpp settings.cpp dmxdaemon.cpp stringhelper.cpp \
		serverdaemon.cpp ipc.cpp datahelper.cpp dmxcontrol.cpp \
		messages.cpp artnet.cpp

# Directory where the dependecy files are stored
DEPDIR=.deps

# libraries
LIBS=-lwiringPi \
	`pkg-config --static --libs libartnet`

# includes
INCL=-I/usr/include/ \
	`pkg-config --static --cflags libartnet`

# output object files
COBJS:= $(CSRCS:.c=.o)
CPPOBJS:= $(CPPSRCS:.cpp=.o)

# linker flags
LDFLAGS= -pg $(LIBS)

# C compiler flags
CFLAGS= -std=gnu11 -c -pg -g -O0 $(INCL)
# C++ compiler flags
CPPFLAGS= -std=gnu++11 -c -pg -g -O0 $(INCL)

# C compiler
CC:=gcc

# C++ compiler
CPP:=g++

# Linker
LD:=g++

# all target
all: $(OUTPUT) $(DEPDIR)

# install 
install: all
	cp $(OUTPUT) /usr/bin/$(OUTPUT)
	cp $(OUTPUT).conf /etc/$(OUTPUT).conf
	cp $(OUTPUT).service /usr/lib/systemd/system/$(OUTPUT).service
	chown root:root /usr/lib/systemd/system/$(OUTPUT).service
	systemctl enable $(OUTPUT)
	
# uninstall
uninstall: 
	systemctl disable $(OUTPUT)
	rm /usr/bin/$(OUTPUT)
	rm /etc/$(OUTPUT).conf
	rm /usr/lib/systemd/system/$(OUTPUT).service
	

# Create C dependencies
$(DEPDIR)/%.d: %.c $(DEPDIR)
	$(CC) -M $(CFLAGS) $< -o $@
	
# Create C++ dependencies
$(DEPDIR)/%.d: %.cpp $(DEPDIR)
	$(CPP) -M $(CPPFLAGS)  $< -o $@
	
# Compile C source files
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@ $(INCL)
	
# Compile C++ source files
%.o: %.cpp
	$(CPP) $(CPPFLAGS) $< -o $@ $(INCL)

# Make dependency directory
$(DEPDIR):
	@mkdir $(DEPDIR)

# Link object files together
$(OUTPUT$): $(COBJS) $(CPPOBJS)
	$(CPP) $(LDFLAGS) $(COBJS) $(CPPOBJS) -o $@

# Clean all object files and compiled output
.PHONY: clean clean-deps
clean: clean-deps
	@rm -f $(COBJS) $(CPPOBJS)
	@rm -f $(OUTPUT)
	
#Clean all dependencies
clean-deps:
	@rm -f $(CSRCS:%.c=$(DEPDIR)/%.d)
	@rm -f $(CPPSRCS:%.cpp=$(DEPDIR)/%.d)
	
#include dependencies files
include $(CSRCS:%.c=$(DEPDIR)/%.d)
include $(CPPSRCS:%.cpp=$(DEPDIR)/%.d)

