#include <vector>

#include "DirectMidi/CDirectMidi.h"
#include "MyMidi.h"
#include "MyCriticalSection.h"

using namespace std;
using namespace directmidi;

// Maximum size for SysEx data in input port
#define SYSTEM_EXCLUSIVE_MEM 48000

extern CDirectMusic gDMusic;
extern COutputPort gOutPort;

MyCriticalSection gLock;
list<MidiMessage> gMessages;

class MyReceiver: public CReceiver
{
public:

	int input;

	virtual void RecvMidiMsg(REFERENCE_TIME rt,DWORD dwChannel,DWORD dwBytesRead,BYTE *lpBuffer) {}

	virtual void RecvMidiMsg(REFERENCE_TIME rt,DWORD dwChannel,DWORD dwMsg) {
		gLock.enter();

		MidiMessage msg;
		CInputPort::DecodeMidiMsg(dwMsg,&msg.command,&msg.channel,&msg.data1,&msg.data2);
		msg.input = input;
		gMessages.push_back(msg);

		gLock.leave();
	}
};

class MidiSource {
public:

	CInputPort port;
	MyReceiver reciver;
};

vector<MidiSource> gSources;

int MyMidi_StartReceiving (int redirect)
{
	/*
	HMIDIIN handle;
    MMRESULT Result = ::midiInOpen(&handle, 0, NULL, NULL, CALLBACK_NULL);

    if(Result == MMSYSERR_NOERROR)
    {
		midiInStart(handle);
		midiInStop(handle);
		midiInReset(handle);
		midiInClose(handle);
	}
	*/

	// Stop all old ports
	MyMidi_StopReceiving ();

	{
		CInputPort temp;
		temp.Initialize(gDMusic);
		gSources.resize(temp.GetNumPorts());
	}

	for (DWORD i = 0; i < gSources.size(); ++i)
	{
		try
		{
			INFOPORT info;
			gSources[i].port.Initialize(gDMusic);
			gSources[i].port.GetPortInfo(i+1, &info);
			printf ("MyMidi: Opened input port #%d: %s\n", i, info.szPortDescription);

			gSources[i].port.ActivatePort(&info, SYSTEM_EXCLUSIVE_MEM);

			gSources[i].reciver.input = i;
			gSources[i].port.SetReceiver(gSources[i].reciver);

			gSources[i].port.ActivateNotification();
			if (redirect) gSources[i].port.SetThru(0,0,0, gOutPort);
		}
		catch (CDMusicException& e)
		{
			MyMidi_SetLastError(__FUNCTION__, e.GetErrorDescription());
		}
	}

	return 1;
}

void MyMidi_StopReceiving ()
{
	for (DWORD i = 0; i < gSources.size(); ++i)
	{
		try
		{
			gSources[i].port.BreakThru(0,0,0);
			gSources[i].port.TerminateNotification();
			gSources[i].port.ReleasePort();
		}
		catch (CDMusicException& e)
		{
			MyMidi_SetLastError(__FUNCTION__, e.GetErrorDescription());
		}
	}

	gSources.clear();
}

int MyMidi_PopMessage (MidiMessage* message)
{

	gLock.enter();

	if (gMessages.empty())
	{
		gLock.leave();
		return 0;
	}


	*message = gMessages.front();
	gMessages.pop_front();

	gLock.leave();
	return 1;
}

const char* MyMidi_GetInputName (unsigned int input)
{
	CInputPort temp;
	temp.Initialize(gDMusic);

	if (input >= temp.GetNumPorts())
		return NULL;

	INFOPORT info;
	temp.GetPortInfo(input+1, &info);

	static string name;
	name = info.szPortDescription;
	return name.c_str();
}

int MyMidi_GetDeviceCount ()
{
	CInputPort temp;
	temp.Initialize(gDMusic);
	return temp.GetNumPorts();
}
