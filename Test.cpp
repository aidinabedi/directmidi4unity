// ANSI I/0 headers
#include <conio.h>
#include <stdio.h>

#pragma comment (lib,"Debug/MyMidi.lib")
#include "MyMidi.h"

#include "DirectMidi/CDirectMidi.h"
using namespace directmidi;

int main()
{
	MyMidi_Init();

	MidiMessage message;

	//MyMidi_LoadDLS(NULL, 44100);
	MyMidi_LoadDLS("media/SteinwayJazzPiano.dls", 44100);
	MyMidi_LoadInstrument(0, 0);
	MyMidi_StartReceiving(1);

	MyMidi_SendMessage(PATCH_CHANGE,0,0,0);
	//MyMidi_SendMessage(NOTE_ON,0,60,127);

	printf("Press 'r' to restart or any other key to quit...\n");
	while (!_kbhit())
	{
		while (MyMidi_PopMessage (&message) != 0)
		{
			printf("command: %d, data1: %d, data2: %d, input: %d\n",
				message.command, message.data1, message.data2, message.input);
		}
	}

	//MyMidi_SendMessage(NOTE_OFF,0,60,0); 
	MyMidi_StopReceiving();
	MyMidi_UnloadDLS();

	return getch() == 'r' ? main():0;
}
