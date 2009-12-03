#pragma once

#define DLLEXPORT __declspec(dllexport)

struct MidiMessage
{
	unsigned char command;
	unsigned char channel;
	unsigned char data1;
	unsigned char data2;
	unsigned int input;
};   

extern "C"
{
///////////////////////////////////////////////////////////////////////////////

	DLLEXPORT int MyMidi_StartReceiving (int redirect);
	DLLEXPORT void MyMidi_StopReceiving ();

	DLLEXPORT int MyMidi_PopMessage (MidiMessage* message);

	DLLEXPORT const char* MyMidi_GetInputName (unsigned int input);
	DLLEXPORT int MyMidi_GetInputCount ();

///////////////////////////////////////////////////////////////////////////////

	DLLEXPORT int MyMidi_LoadDLS (const char* filename, int sampleRate);
	DLLEXPORT int MyMidi_UnloadDLS ();

	DLLEXPORT const char* MyMidi_GetInstrumentName (unsigned int index);
	DLLEXPORT unsigned int MyMidi_GetInstrumentCount ();

	DLLEXPORT int MyMidi_LoadInstrument (unsigned int index, int patch);
	DLLEXPORT int MyMidi_UnloadInstrument (unsigned int index);

	DLLEXPORT int MyMidi_SendMessage (char command, char channel, char data0, char data1);

///////////////////////////////////////////////////////////////////////////////

	DLLEXPORT int MyMidi_Init ();

	DLLEXPORT const char* MyMidi_GetLastError ();
	DLLEXPORT void MyMidi_SetLastError (const char* source, const char* description);
	DLLEXPORT void MyMidi_ResetLastError ();

///////////////////////////////////////////////////////////////////////////////
}
