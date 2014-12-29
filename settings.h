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
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <string>
#include <list>
#include <istream>


class Settings
{
public:
	
	static void SetFileName(std::string file);
	
	static void Load(int argc = 0, char **argv = NULL);
	static void Save();
	
	static int GetSPIPort();
	static int GetSPISpeed();
	static int GetServerUser();
	static int GetDMXUser();
	
	static std::string GetArtNetIp();
	static std::string GetArtNetLongName();
	static std::string GetArtNetShortName();
	static std::string GetInitialState();
	

private:
	static void GetContent(std::list<std::string> *list, std::ifstream &file);
	static int ReadInt(std::string str, int def);
	static float ReadFloat(std::string str, float def);
	static std::string ReadString(std::string str, std::string def);
	static int ReadUID(std::string str, int def);
	
	static int mPortNum;
	static int mPortSpeed;
	static std::string mServerUser;
	static std::string mDMXUser;
	static std::string mArtNetIp;
	static std::string mArtNetLongName;
	static std::string mArtNetShortName;
	static std::string mInitialState;
	static std::string mFileName;

};

#endif
