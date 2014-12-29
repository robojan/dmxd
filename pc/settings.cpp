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

#include <exception>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <pwd.h>
#include <cassert>
#include <getopt.h>
#include "settings.h"
#include "global.h"
#include "logger.h"
#include "stringhelper.h"

using namespace std;

int Settings::mPortNum = DEFAULT_PORTNUM;
int Settings::mPortSpeed = DEFAULT_PORTSPEED;
std::string Settings::mServerUser = DEFAULT_SERVERUSER;
std::string Settings::mDMXUser = DEFAULT_DMXUSER;
std::string Settings::mArtNetIp = DEFAULT_ARTNETIP;
std::string Settings::mArtNetLongName = DEFAULT_ARTNETLONGNAME;
std::string Settings::mArtNetShortName = DEFAULT_ARTNETSHORTNAME;
std::string Settings::mInitialState = DEFAULT_INITIALSTATE;
std::string Settings::mFileName = CONFIG_FILE;

void Settings::SetFileName(std::string fileName)
{
	mFileName = fileName;
}

void Settings::GetContent(std::list<std::string> *list, std::ifstream &file)
{
	string line;

	assert(list != NULL);
	assert(file.is_open());

 	// Goto the beginning of the file
	file.seekg(0);

	// Clear the contents of the list
	list->clear();
	
	// Read the whole file.
	getline(file, line);
	while(file.good())
	{
		list->push_back(line);
		getline(file,line);
	}
	
}

int Settings::ReadInt(std::string str, int def)
{
	int result = def;
	try {
		result = stoi(str);
	} catch(invalid_argument ) {
		cerr << "Warning: could not read integer in \"" << str << "\" using default: " << def << endl;
		result = def;
	} catch(out_of_range) {
		cerr << "Warning: could not read integer in \"" << str << "\" using default: " << def << endl;
		result = def;
	}
	return result;
}

float Settings::ReadFloat(std::string str, float def)
{
	float result = def;
	try {
		result = stof(str);
	} catch(invalid_argument ) {
		cerr << "Warning: could not read float in \"" << str << "\" using default: " << def << endl;
		result = def;
	} catch(out_of_range) {
		cerr << "Warning: could not read float in \"" << str << "\" using default: " << def << endl;
		result = def;
	}
	return result;
}

std::string Settings::ReadString(std::string str, std::string def)
{
	if(str.empty())
	{
		cerr << "Warning: could not read string in \"" << str << "\" using default: " << def << endl;
		return def;
	} else {
		return str;
	}
}

int Settings::ReadUID(std::string str, int def)
{
	int id = -1;
	struct passwd *result;
	try {
		id = stoi(str);
		return id;
	} catch(invalid_argument ) {
		result = getpwnam(str.c_str());
	} catch(out_of_range) {
		result = getpwnam(str.c_str());
	}
	if(id == -1 && result == NULL)
	{
		cerr << "Warning: Could not find user \"" << str << "\" defaulting to root" << endl;
		return 0;
	}
	return result->pw_uid;
}

void Settings::Load(int argc, char **argv)
{
	bool foption = false;
	// options
	static struct option options[] = {
		{"file", required_argument, NULL, 'f'},
		{"port", required_argument, NULL, 'p'},
		{"speed", required_argument, NULL, 's'}, 
		{"dmxuser", required_argument, NULL, 'd'}, 
		{"initial", required_argument, NULL, 'i'},
		{"serveruser", required_argument, NULL, 'u'}, 
		{"bind", required_argument, NULL, 'b'},
		{"help", no_argument, NULL, 'h'},
		{0,0,0,0}
	};
	// Get command line options
	if(argv != NULL && argc != 0)
	{
		int c;
		int index = 0;

		while((c = getopt_long(argc, argv, "f:p:s:d:i:u:b:h", options, &index))!= -1)
		{
			switch(c)
			{
			case 'f':
				foption = true;
				SetFileName(optarg);
				break;
			case 'h':
				// Print usage
				printf("Usage %s [-f configfile][-p port_number][-d dmx_user]\n"
					"\t\t[-i {on,off}][-u artnet_user][-b bind_ip][-h]\n\n"
					"\t --file configfile:-f: Path to config file\n"
					"\t--port port_number:-p: Port number of the SPI device\n"
					"\t     --speed speed:-s: Speed of the SPI interface in kHz\n"
					"\t    --dmxuser user:-d: User in which the DMX daemon runs\n"
					"\t--initial {on,off}:-i: Initial state off all the channels\n"
					"\t --serveruser user:-u: User in which the Art-Net node runs\n"
					"\t         --bind ip:-b: Ip address that the Art-Net node binds to\n"
					"\t            --help:-h: Show this help\n", argv[0]);
				exit(0);
			default: 
				// only handle file now
				break;
			}
		}
	}
	// Load or create default
	ifstream file(mFileName.c_str());
	if(file.is_open())
	{
		list<string> content;	
		// Read the entire file line by line
		GetContent(&content, file);

		file.close();

		// Read the content of the file
		for(list<string>::iterator it = content.begin(); it != content.end(); ++it)
		{
			string str = TrimString(*it, " \t\n\r");
			size_t posEqual = str.find_first_of('=');
			// Check if string is empty
			if(str.empty()|| str[0] == '#')
			{
				continue;
			}
			if(posEqual == string::npos)
			{
				cerr << "Error in configuration file at: \"" << *it << "\"" << endl;
				continue;
			}
			
			// Get key
			string key = str.substr(0, posEqual);
			string value = str.substr(posEqual+1);
			key = TrimString(key, " \t\n\r");
			value = TrimString(value, " \t\n\r");

			// Get the configuration
			if(key == "SPIPort")
			{
				mPortNum = ReadInt(value, DEFAULT_PORTNUM);
			} else if( key == "SPISpeed" ) {
				mPortSpeed =  ReadInt(value, DEFAULT_PORTSPEED);
			} else if( key == "SPIUser" ) {
				mDMXUser = ReadString(value, DEFAULT_DMXUSER);
			} else if( key == "InitialState" ) {
				mInitialState = ReadString(value, DEFAULT_INITIALSTATE);
			} else if( key == "ServerUser" ) {
				mServerUser = ReadString(value, DEFAULT_SERVERUSER);
			} else if( key == "ArtNetIp" ) {
				mArtNetIp = ReadString(value, DEFAULT_ARTNETIP);
			} else if( key == "ArtNetLongName" ) {
				mArtNetLongName = ReadString(value, DEFAULT_ARTNETLONGNAME);
			} else if( key == "ArtNetShortName" ) {
				mArtNetShortName = ReadString(value, DEFAULT_ARTNETSHORTNAME);
			} else {
				cerr << "Unknown key found in configuration file: \"" << key << "\"" << endl;
				continue;
			}
		}	
	} else {
		if(foption)
		{
			Error("Could not open file %s", mFileName.c_str());
			return;
		}
	}
	
	// Get command line options
	if(argv != NULL && argc != 0)
	{
		int c;
		int index = 0;
		// Reset parser
		optind = 1;
		while((c = getopt_long(argc, argv, "f:p:s:d:i:u:b:h", options, &index))!= -1)
		{
			switch(c)
			{
			case 'p':
				mPortNum = ReadInt(optarg, DEFAULT_PORTNUM);
				break;
			case 's':
				mPortSpeed = ReadInt(optarg, DEFAULT_PORTSPEED);
				break;
			case 'd':
				mDMXUser = ReadString(optarg, DEFAULT_DMXUSER);
				break;
			case 'i':
				mInitialState = ReadString(optarg, DEFAULT_INITIALSTATE);
				break;
			case 'u':
				mServerUser = ReadString(optarg, DEFAULT_SERVERUSER);
				break;
			case 'b':
				mArtNetIp = ReadString(optarg, DEFAULT_ARTNETIP);
				break;
			default: 
				// only handle file now
				break;
			}
		}
	}
}

void Settings::Save()
{
	list<string> content;	
	ifstream iFile(mFileName.c_str());
	if(iFile.is_open())
	{
		// Read the entire file line by line
		GetContent(&content, iFile);
		iFile.close();
	} else {
		// Fill the content manually
		content.push_back("#\n# DMX daemon configuration file\n#\n");
		content.push_back("########## SPI settings ##########");
		content.push_back("# The SPI port number. (0 or 1)");
		content.push_back("SPIPort = 0");
		content.push_back("\n# The SPI port speed in kHz. ");
		content.push_back("SPISpeed = 60");
		content.push_back("\n# The user that is used for the dmx communication");
		content.push_back("SPIUser = root");
		content.push_back("\n########## Server settings ##########");
		content.push_back("# The user that is used for the server communication");
		content.push_back("ServerUser = http");
		content.push_back("# Initial state of channels(on, off)");
		content.push_back("InitialState = on");
		content.push_back("\n########## Art-Net settings ##########");
		content.push_back("# The IP address that Art-Net binds to");
		content.push_back("ArtNetIp = 0.0.0.0");
		content.push_back("# The Long name for the Art-Net node(max 63 characters)");
		content.push_back("ArtNetLongName = SPI-DMX Art-Net daemon http://www.robojan.nl");
		content.push_back("# The short name for the Art-Net node(max 17 characters)");
		content.push_back("ArtNetShortName = SPI-DMX daemon");
	}

	// Modify the content so that it has the current configuration
	for(list<string>::iterator it = content.begin(); it != content.end(); ++it)
	{
		string str = TrimString(*it, " \t\n\r");
		size_t posEqual = str.find_first_of('=');
		// Check if string is empty
		if(str.empty()|| str[0] == '#')
		{
			continue;
		}
		if(posEqual == string::npos)
		{
			cerr << "Error in configuration file at: \"" << *it << "\"" << endl;
			continue;
		}
		
		// Get key
		string key = str.substr(0, posEqual-1);
		key = TrimString(key, " \t\n\r");

		// Create the new line
		stringstream keyValuePair;
		keyValuePair << key << " = ";
		if(key == "SPIPort")
		{
			keyValuePair << mPortNum;
		} else if( key == "SPISpeed" ) {
			keyValuePair << mPortSpeed;
		} else if( key == "SPIUser" ) {
			keyValuePair << mDMXUser;
		} else if( key == "ServerUser" ) {
			keyValuePair << mServerUser; 
		} else if( key == "InitialState" ) {
			keyValuePair << mInitialState;
		} else if( key == "ArtNetIp" ) {
			keyValuePair << mArtNetIp;
		} else if( key == "ArtNetLongName" ) {
			keyValuePair << mArtNetLongName;
		} else if( key == "ArtNetShortName" ) {
			keyValuePair << mArtNetShortName;
		} else {
			cerr << "Unknown key found in configuration file: \"" << key << "\"" << endl;
			continue;
		}
		
		// Save the key value pair
		*it = keyValuePair.str();
	}

	// Create new file
	ofstream ofile(mFileName.c_str(), fstream::trunc | fstream::out);
	if(!ofile.is_open())
	{
		cerr << "Error: Could not open the configuration file" << endl;
		return;
	}
	
	// Write the content to the file
	for(list<string>::iterator it = content.begin(); it!= content.end(); ++it)
	{
		ofile << it->c_str() << "\n";
	}	

	ofile.close();
}

int Settings::GetSPIPort()
{
	return mPortNum;
}

int Settings::GetSPISpeed()
{
	return mPortSpeed;
}

int Settings::GetServerUser()
{
	return Settings::ReadUID(mServerUser, 0);
}

int Settings::GetDMXUser()
{
	return Settings::ReadUID(mDMXUser, 0);
}

std::string Settings::GetArtNetIp()
{
	return mArtNetIp;
}

std::string Settings::GetArtNetLongName()
{
	return mArtNetLongName;
}

std::string Settings::GetArtNetShortName()
{
	return mArtNetShortName;
}

std::string Settings::GetInitialState()
{
	return mInitialState;
}


