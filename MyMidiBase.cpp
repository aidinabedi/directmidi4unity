#include <string>
#include <stdio.h>

#include "DirectMidi/CDirectMidi.h"
#include "MyMidi.h"

#pragma comment (lib,"dxguid.lib") // guid definitions 
#pragma comment (lib,"winmm.lib")
#pragma comment (lib,"dsound.lib")
#pragma comment (lib,"dxerr8.lib")

using namespace std;
using namespace directmidi;

bool gInitialized = false;
CDirectMusic gDMusic;
string gLastError;

int MyMidi_Init ()
{
	if (gInitialized) return 0;
	gInitialized = true;

	try
	{
		gDMusic.Initialize();
		return 1;
	}
	catch (CDMusicException& e)
	{
		MyMidi_SetLastError(__FUNCTION__, e.GetErrorDescription());
	}

	return 0;
}

const char* MyMidi_GetLastError ()
{
	static string buffer = "";
	buffer = gLastError;
	return buffer.c_str();
}

void MyMidi_ResetLastError ()
{
	gLastError = "OK";
}

void MyMidi_SetLastError (const char* source, const char* description)
{
	gLastError = source;
	gLastError += ": ";
	gLastError += description;
	printf("MyMidi Error in %s: %s\n", source, description);
}

