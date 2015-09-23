/*
   RainbowCrack - a general propose implementation of Philippe Oechslin's faster time-memory trade-off technique.

   Copyright (C) Zhu Shuanglei <shuanglei@hotmail.com>
*/

#ifdef _WIN32
	#pragma warning(disable : 4786)
#endif

#include "Public.h"

#ifdef _WIN32
	#include <windows.h>
#else
	#include <sys/sysinfo.h>
#endif

//////////////////////////////////////////////////////////////////////

unsigned int GetFileLen(FILE* file)
{
	unsigned int pos = ftell(file);
	fseek(file, 0, SEEK_END);
	unsigned int len = ftell(file);
	fseek(file, pos, SEEK_SET);

	return len;
}

string TrimString(string s)
{
	while (s.size() > 0)
	{
		if (s[0] == ' ' || s[0] == '\t')
			s = s.substr(1);
		else
			break;
	}

	while (s.size() > 0)
	{
		if (s[s.size() - 1] == ' ' || s[s.size() - 1] == '\t')
			s = s.substr(0, s.size() - 1);
		else
			break;
	}

	return s;
}

bool ReadLinesFromFile(string sPathName, vector<string>& vLine)
{
	vLine.clear();

	FILE* file = fopen(sPathName.c_str(), "rb");
	if (file != NULL)
	{
		unsigned int len = GetFileLen(file);
		char* data = new char[len + 1];
		fread(data, 1, len, file);
		data[len] = '\0';
		string content = data;
		content += "\n";
		delete data;
		//printf("Parsing\n");
		int i;
		int nRead=0;
		for (i = 0; i < content.size(); i++)
		{
			if (content[i] == '\r')
				content[i] = '\n';
			/*	
			if(content[i] == '\n'){
				nRead++;
				int n=i;
				string line = content.substr(0, n);
				line = TrimString(line);
				fprintf(stderr,"%d        \r",nRead);
				if (line != "")
					vLine.push_back(line);
				content = content.substr(n + 1);
				i=0;
			}*/
		}

		int n;
		
		
		while ((n = content.find("\n", 0)) != -1)//longest loop?
		{
			string line = content.substr(0, n);
			line = TrimString(line);
			
			if (line != ""){
				vLine.push_back(line);
				nRead++;
			}
			fprintf(stderr,"%d read\r",nRead);
			content = content.substr(n + 1);
			//content+=n;
		}
		//printf("Done parsing\n");
		fclose(file);
	}
	else
		return false;

	return true;
}

bool SeperateString(string s, string sSeperator, vector<string>& vPart)
{
	vPart.clear();

	int i;
	for (i = 0; i < sSeperator.size(); i++)
	{
		int n = s.find(sSeperator[i]);
		if (n != -1)
		{
			vPart.push_back(s.substr(0, n));
			s = s.substr(n + 1);
		}
		else
			return false;
	}
	vPart.push_back(s);

	return true;
}

string uint64tostr(uint64 n)
{
	char str[32];

#ifdef _WIN32
	sprintf(str, "%I64u", n);
#else
	sprintf(str, "%llu", n);
#endif

	return str;
}

string uint64tohexstr(uint64 n)
{
	char str[32];

#ifdef _WIN32
	sprintf(str, "%016I64x", n);
#else
	sprintf(str, "%016llx", n);
#endif

	return str;
}

string HexToStr(const unsigned char* pData, int nLen)
{
	string sRet;
	int i;
	for (i = 0; i < nLen; i++)
	{
		char szByte[3];
		sprintf(szByte, "%02x", pData[i]);
		sRet += szByte;
	}

	return sRet;
}

unsigned int GetAvailPhysMemorySize()
{
#ifdef _WIN32
		MEMORYSTATUS ms;
		GlobalMemoryStatus(&ms);
		return ms.dwAvailPhys;
#else
	struct sysinfo info;
	sysinfo(&info);			// This function is Linux-specific
	return info.freeram;
#endif
}

void ParseHash(string sHash, unsigned char* pHash, int& nHashLen)
{
	int i;
	for (i = 0; i < sHash.size() / 2; i++)
	{
		string sSub = sHash.substr(i * 2, 2);
		int nValue;
		sscanf(sSub.c_str(), "%02x", &nValue);
		pHash[i] = (unsigned char)nValue;
	}

	nHashLen = sHash.size() / 2;
}

void Logo()
{
	CI_Logo();
	
	printf("RainbowCrack 1.2 - Making a Faster Cryptanalytic Time-Memory Trade-Off\n");
	printf("by Zhu Shuanglei <shuanglei@hotmail.com>\n");
	printf("http://www.antsight.com/zsl/rainbowcrack/\n\n");
	
	printf("Modiffied by KALGECIN\n");
	printf("kalgecin@gmail.com\n\n");
}

/*
* NAME: CI_Logo()
* PURPOSE: Prints out CI Logo
* RETURNS: N/A
* PROVIDED BY: Andrew Malone
* DATE: 9/23/2015
*/

void CI_Logo()
{
	printf("\033[0;32m"); // Print out GREEN text
	printf(" CCCCC       IIIIIIIII\n");
	printf("CCC CCC         III\n");
	printf("CCC CCC         III\n");
	printf("CCC             III\n");
	printf("CCC     ====    III\n");
	printf("CCC     ====    III\n");
	printf("CCC             III\n");
	printf("CCC CCC         III\n");
	printf("CCC CCC         III\n"); //underline the Web Address
	printf(" CCCCC       IIIIIIIII   \033[4;33mhttp://ci-main.no-ip.org/\n"); 
	printf("\033[0m\n")
}

/*
* NAME: GetNodeName()
* PURPOSE: Grabs DNS Names for the computational node
* RETURNS: char* (string)
* PROVIDED BY: Andrew Malone
* DATE: 9/23/2015
*/

char* GetNodeName()
{
	struct addrinfo hints, *info, *p;
	int gai_result;

	char hostname[1024];
	hostname[1023] = '\0';
	gethostname(hostname, 1023);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_result));
		exit(1);
	}

	for(p = info; p != NULL; p = p->ai_next) {
		return p->ai_canonname;
	}	

	freeaddrinfo(info);
}