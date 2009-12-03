#include <vector>

#include "DirectMidi/CDirectMidi.h"
#include "MyMidi.h"

using namespace std;
using namespace directmidi;

extern CDirectMusic gDMusic;

COutputPort gOutPort;
CDLSLoader gLoader;
CCollection gCollection;
vector<CInstrument> gInstruments;

int MyMidi_LoadDLS (const char* filename, int sampleRate)
{
	MyMidi_ResetLastError();
	MyMidi_UnloadDLS();

	try
	{
		gOutPort.Initialize(gDMusic); // it's ok to reinitialize COutputPort

		INFOPORT PortInfo;
		DWORD dwPortCount = 0;

		// selection first Software Synthesizer
		do
			gOutPort.GetPortInfo(++dwPortCount,&PortInfo);
		while (!(PortInfo.dwFlags & DMUS_PC_SOFTWARESYNTH));

		gOutPort.SetPortParams(0,0,1,SET_REVERB | SET_CHORUS,sampleRate);
		gOutPort.ActivatePort(&PortInfo);
		printf("MyMidi: Selected output port: %s\n", PortInfo.szPortDescription);

		gLoader.Initialize();		
		gLoader.LoadDLS((LPSTR) filename, gCollection);	

		INSTRUMENTINFO InstInfo;
		DWORD dwInstIndex = 0;
		
		while (gCollection.EnumInstrument(dwInstIndex,&InstInfo) == S_OK)
		{
			printf("MyMidi: Found instrument #%u: %s\n", dwInstIndex, InstInfo.szInstName);
			dwInstIndex++;
		}

		gInstruments.resize(dwInstIndex);

		for (DWORD i = 0; i < gInstruments.size(); i++)
		{
			gCollection.GetInstrument(gInstruments[i],i);
			gInstruments[i].SetNoteRange(0,0);
		}

		return 1;
	}
	catch (CDMusicException& e)
	{
		MyMidi_SetLastError(__FUNCTION__, e.GetErrorDescription());
	}

	return 0;
}

int MyMidi_UnloadDLS ()
{
	MyMidi_ResetLastError();

	if (gInstruments.empty())
		return 1;

	try
	{
		gLoader.UnloadCollection(gCollection);

		DWORD lowNote, highNote;

		for (DWORD i = 0; i < gInstruments.size(); i++)
		{
			gInstruments[i].GetNoteRange(&lowNote, &highNote);
			if (lowNote == 0 && highNote == 127)
				gOutPort.UnloadInstrument(gInstruments[i]);
		}

		gInstruments.clear();

		gOutPort.ReleasePort();
		return 1;
	}
	catch (CDMusicException& e)
	{
		MyMidi_SetLastError(__FUNCTION__, e.GetErrorDescription());
	}

	return 0;
}

unsigned int MyMidi_GetInstrumentCount ()
{
	MyMidi_ResetLastError();
	return gInstruments.size();
}

const char* MyMidi_GetInstrumentName (unsigned int index)
{
	MyMidi_ResetLastError();

	if (index >= gInstruments.size())
	{
		MyMidi_SetLastError(__FUNCTION__, "Index out of range");
		return NULL;
	}

	return gInstruments[index].m_strInstName;
}

int MyMidi_LoadInstrument (unsigned int index, int patch)
{
	MyMidi_ResetLastError();

	if (index >= gInstruments.size())
	{
		MyMidi_SetLastError(__FUNCTION__, "Index out of range");
		return 0;
	}

	MyMidi_UnloadInstrument(index);

	try
	{
		gInstruments[index].SetPatch(patch);
		gInstruments[index].SetNoteRange(0,127);
		gOutPort.DownloadInstrument(gInstruments[index]);

		return 1;
	}
	catch (CDMusicException& e)
	{
		MyMidi_SetLastError(__FUNCTION__, e.GetErrorDescription());
	}

	return 0;
}

int MyMidi_UnloadInstrument (unsigned int index)
{
	MyMidi_ResetLastError();

	if (index >= gInstruments.size())
	{
		MyMidi_SetLastError(__FUNCTION__, "Index out of range");
		return 0;
	}

	try
	{
		DWORD lowNote, highNote;

		gInstruments[index].GetNoteRange(&lowNote, &highNote);
		if (lowNote == 0 && highNote == 127)
		{
			gOutPort.UnloadInstrument(gInstruments[index]);
			gInstruments[index].SetNoteRange(0, 0);
		}

		return 1;
	}
	catch (CDMusicException& e)
	{
		MyMidi_SetLastError(__FUNCTION__, e.GetErrorDescription());
	}

	return 0;
}

int MyMidi_SendMessage (char command, char channel, char data0, char data1)
{
	MyMidi_ResetLastError();

	try
	{
		gOutPort.SendMidiMsg(COutputPort::EncodeMidiMsg(command,channel,data0,data1),0);
		return 1;
	}
	catch (CDMusicException& e)
	{
		MyMidi_SetLastError(__FUNCTION__, e.GetErrorDescription());
	}

	return 0;
}
