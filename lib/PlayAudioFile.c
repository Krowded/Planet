//Plays music using system call to hidden vlc player, and hiding terminal output.
//Not pretty, not portable

#include <stdlib.h>
#include <string.h>
#include "PlayAudioFile.h"

#define LOCAL static

const char* programstring = "cvlc --play-and-exit --quiet ";
const char* hideConsoleString = " > /dev/null 2>&1&";

const char* programstringBackgroundMusic = "cvlc --repeat --quiet "; //Repeats the background music

void PlayAudioFile(char filename[])
{	
	char command[strlen(filename)+ strlen(programstring) + strlen(hideConsoleString)];
	strcpy(command, programstring);
	strncat(command, filename, strlen(filename));
	strncat(command, hideConsoleString, strlen(hideConsoleString));
	system(command);
}

void KillAllAudio()
{
	system("pkill -f vlc");
}