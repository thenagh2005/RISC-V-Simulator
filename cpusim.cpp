

#include <iostream>
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

/*
Add all the required standard and developed libraries here
*/
#include "Control.h"
#include "CPU.h"
#include "ALU.h"
#include "MUX.h"

static inline bool isHexChar(char c)
{
	return (c >= '0' && c <= '9') ||
		   (c >= 'a' && c <= 'f') ||
		   (c >= 'A' && c <= 'F');
}
static inline int hexVal(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return 10 + (c - 'a');
	if (c >= 'A' && c <= 'F')
		return 10 + (c - 'A');
	return 0;
}

static inline uint32_t parseHex(const string &s)
{
	uint32_t v = 0;
	for (char c : s)
	{
		v = (v << 4) | hexVal(c);
	}
	return v;
}
static inline string stripNonHex(const string &tok)
{
	string out;
	for (char c : tok)
		if (isHexChar(c))
			out.push_back(c);
	return out;
}
static inline int32_t signExtend(uint32_t val, int bits)
{
	// sign extend 'bits' bits in val into 32-bit signed int
	uint32_t mask = 1u << (bits - 1);
	if (val & mask)
	{
		uint32_t extend = (~0u) << bits;
		return (int32_t)(val | extend);
	}
	else
	{
		return (int32_t)val;
	}
}

// Aux function to parse an input file
vector<uint8_t> parseInputFile(const string &path)
{
	ifstream ifs(path);
	if (!ifs)
	{
		cerr << "Error: could not open input file: " << path << "\n";
		exit(1);
	}
	vector<uint8_t> bytes;
	string tok;
	while (ifs >> tok)
	{
		string hx = stripNonHex(tok);
		if (hx.empty())
			continue;
		// Some tokens might be long like 09a06293 (instruction), or single bytes '93'
		if (hx.size() == 8)
		{
			// treat as 4 bytes hex representing 32-bit instruction; token likely in big-endian text
			// We must push bytes in little-endian order (low-order byte first).
			// hx e.g. "09a06293" -> bytes: 0x93,0x62,0xa0,0x09
			for (int i = 6; i >= 0; i -= 2)
			{
				string byteStr = hx.substr(i, 2);
				uint8_t b = (uint8_t)parseHex(byteStr);
				bytes.push_back(b);
			}
		}
		else if (hx.size() == 4)
		{
			// treat as two bytes; push as little-endian (low-order first)
			// e.g. "6293" -> bytes 0x93,0x62
			string b0 = hx.substr(2, 2);
			string b1 = hx.substr(0, 2);
			bytes.push_back((uint8_t)parseHex(b0));
			bytes.push_back((uint8_t)parseHex(b1));
		}
		else
		{
			// length 1..2 etc -- parse as byte (pad if single digit)
			if (hx.size() == 1)
				hx = "0" + hx;
			// if hx longer than 2 (e.g., '00000000') but not 4/8 handled above, we chunk it into pairs from end
			if (hx.size() > 2)
			{
				// chunk from end in pairs
				int len = (int)hx.size();
				// push bytes in natural order if hx is already one-byte per token? Better safe:
				// take last two, then previous two...
				for (int i = len - 2; i >= 0; i -= 2)
				{
					string bs = hx.substr(i, 2);
					uint8_t b = (uint8_t)parseHex(bs);
					bytes.push_back(b);
				}
			}
			else
			{
				uint8_t b = (uint8_t)parseHex(hx);
				bytes.push_back(b);
			}
		}
	}
	return bytes;
}

/*
Put/Define any helper function/definitions you need here
*/

unsigned char hexCharToByte(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return 10 + (c - 'a');
	if (c >= 'A' && c <= 'F')
		return 10 + (c - 'A');
	return 0;
}

int main(int argc, char **argv)
{
	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	if (argc < 2)
	{
		cerr << "Not enough args" << endl;
		return 1;
	}

	string path = argv[1];

	vector<uint8_t> programBytes = parseInputFile(path); // Parse file

	// Initialize CPU and run program
	CPU cpu;
	cpu.loadBytes(programBytes);
	cpu.runCycle();

	// Get results

	int32_t a0 = cpu.getRegister(10);
	int32_t a1 = cpu.getRegister(11);

	// Print, and we are done!

	cout << "(" << a0 << "," << a1 << ")\n";
	return 0;
}
