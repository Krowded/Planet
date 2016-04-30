#include <stdlib.h>
#include <string.h>
#include "PlayAudioFile.h"
#include <stdio.h>
#include <pthread.h>

#define LOCAL static

const char* programstring = "cvlc --play-and-exit --quiet ";

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
	}
}

LOCAL void* ActuallyPlayFile(void* filename)
{
	char* Filename = (char*)filename;
	char command[strlen(filename)+ strlen(programstring)];
	strcpy(command, programstring);
	strncat(command, filename, strlen(filename));
	system(command);
}

LOCAL void AddThreadToList(pthread_t newThread)
{
	numberOfThreads++;
	threadList = realloc(threadList, sizeof(pthread_t)*numberOfThreads);
	threadList[numberOfThreads] = newThread;
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
{
	int counter = numberOfThreads;
	int i;
	for(i = 0; i < counter; i++)
	{
		DeleteLastThread();
	}
	UglyKillAllSolution();
}

LOCAL void UglyKillAllSolution()
{
	system("pkill -f vlc");
}