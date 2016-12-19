#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/neutrino.h>

#include "Assignment1-Common.h"

int inputChannelID = 0; //Global channel ID for messages from controller (accessed from signal handlers and exit function)

void displayMessage (StatusMessage * message); //Display message from controller

void signalCleanup (int signal); //Clean up resources if process is killed
void exitCleanup (void); //Clean up resources if we exit for any reason

int main (void)
{
	if (atexit (exitCleanup) != 0)
		otherError ("Could not register atexit cleanup function");

	//Destroy message channel if process is killed (register cleanupResources as a handler for terminating signals)
	struct sigaction handleExit;
	handleExit.sa_handler = signalCleanup;
	sigfillset (&handleExit.sa_mask);
	handleExit.sa_flags = 0;

	//Keyboard interrupt (SIGINT), hangup (SIGHUP), terminate  (SIGTERM)
	if (sigaction (SIGINT, &handleExit, NULL) < 0)
		otherError ("Cannot register SIGINT handler for input program");
	if (sigaction (SIGHUP, &handleExit, NULL) < 0)
		otherError ("Cannot register SIGUP handler for input program");
	if (sigaction (SIGTERM, &handleExit, NULL) < 0)
		otherError ("Cannot register SIGTERM handler for input program");

	pid_t serverPID = getpid ();

	printf ("Display server started with PID: %d\n", serverPID);

	inputChannelID = ChannelCreate (0);
	if (inputChannelID == -1)
		otherError ("Could not create channel for display server");

	int messageID = 0;
	StatusMessage message;
	int replyValue = 0; //Throwaway reply value

	while (1)
	{
		messageID = MsgReceive (inputChannelID, &message, sizeof (message), NULL);
		if (messageID < 0)
			otherError ("Error receving output message");

		if (MsgReply (messageID, 0, &replyValue, sizeof (replyValue)) < 0)
			otherError ("Could not reply to output message");

		displayMessage (&message);
	}

	return EXIT_SUCCESS;
}

void displayMessage (StatusMessage * message)
{
	if (message == NULL)
		return;

	printf ("\nController sent message: %s\n", message->message);
}

void signalCleanup (int signal)
{
	ChannelDestroy (inputChannelID);

	exit (0); //Not sure if necessary, do kill signals still kill the process if I override the default handler?
}

void exitCleanup (void)
{
	ChannelDestroy (inputChannelID);
}
