//Plays music using system call to hidden vlc on different thread.
//Not pretty, not at all portable



#include <stdlib.h>
#include <string.h>
#include "PlayAudioFile.h"
#include <stdio.h>
#include <pthread.h>

#define LOCAL static

const char* programstring = "cvlc --play-and-exit --quiet ";
const char* programstringBackgroundMusic = "cvlc --repeat --quiet ";
const char* hideConsoleString = " > /dev/null 2>&1";

LOCAL void AddThreadToList(pthread_t newThread);
LOCAL void* ActuallyPlayFile(void* filename);
LOCAL void UglyKillAllSolution();

pthread_t* threadList;
int numberOfThreads = 0;

LOCAL void* ActuallyPlayFile(void*);

void PlayAudioFile(char filename[])
{	
	pthread_t newThread;

	int check = pthread_create(&newThread, NULL, &ActuallyPlayFile, filename);

	if(check != 0)
	{
		fprintf(stderr, "Thread create failed");
	}
	else
	{
		AddThreadToList(newThread);
		pthread_detach(newThread);

	}
}

void DoNothing(){};


LOCAL void* ActuallyPlayFile(void* filename)
{
	atexit(DoNothing); //Or it will shutdown the entire program

	char* Filename = (char*)filename;
	char command[strlen(filename)+ strlen(programstring) + strlen(hideConsoleString)];
	strcpy(command, programstring);
	strncat(command, filename, strlen(filename));
	strncat(command, hideConsoleString, strlen(hideConsoleString));
	system(command);
}

LOCAL void AddThreadToList(pthread_t newThread)
{
	/*numberOfThreads++;
	threadList = realloc(threadList, sizeof(pthread_t)*numberOfThreads);
	threadList[numberOfThreads] = newThread;*/
}

//void DeleteSpecificThread()

void DeleteLastThread()
{
	pthread_t tempThread = threadList[numberOfThreads];
	pthread_cancel(tempThread);
	pthread_join(tempThread, NULL);
	numberOfThreads--;
}

void DeleteAllThreads()
{/*
	int counter = numberOfThreads;
	int i;
	for(i = 0; i < counter; i++)
	{
		DeleteLastThread();
	}*/
	UglyKillAllSolution();
}

LOCAL void UglyKillAllSolution()
{
	system("pkill -f vlc");
}