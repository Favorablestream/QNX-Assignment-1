#include <stdlib.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <unistd.h>

#include "Controller-State-Machine.h"

int inputChannelID = 0; //Global channel ID for messages from input program (accessed from signal handlers and exit function)
int outputConnectionID = 0; //Global connection ID for messages to display server (accessed from signal handlers and exit function)

void signalCleanup (int signal); //Clean up resources if process is killed
void exitCleanup (void); //Clean up resources if we exit for any reason

int main (int argc, char ** argv)
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

	if (argc != 2)
		argError ("Expected 1 argument", "Assignment1-Controller displayserver-pid", "./Assignment1-Controller 77234");

	pid_t displayPID = parseNumArg (argv [1]);
	pid_t controllerPID = getpid ();

	printf ("Controller started with PID: %d\n", controllerPID);

	inputChannelID = ChannelCreate (0);
	if (inputChannelID < 0)
		otherError ("Could not create channel for controller");

	outputConnectionID = ConnectAttach (ND_LOCAL_NODE, displayPID, DISPLAY_CHANNEL_ID, _NTO_SIDE_CHANNEL, 0);
	if (outputConnectionID < 0)
		otherError ("Couldn't attach message connection to display server");

	InputMessage nextMessage; //Message that gets overwritten by state handlers with the next input message
	StateHandler nextState; //State handler struct
	nextState.handler = start; //First state is idle

	//State machine loop
	while (1)
	{
		nextState = nextState.handler (&nextMessage, inputChannelID, outputConnectionID);

		delay (500);
	}

	return EXIT_SUCCESS;
}

void signalCleanup (int signal)
{
	ChannelDestroy (inputChannelID);

	ConnectDetach (outputConnectionID);

	exit (0); //Not sure if necessary, do kill signals still kill the process if I override the default handler?
}

void exitCleanup (void)
{
	ChannelDestroy (inputChannelID);

	ConnectDetach (outputConnectionID);
}
