#include <stdlib.h>
#include <stdio.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <string.h>

#include "Assignment1-Common.h"

int connectionID = 0; //Global connection ID for messages to controller (accessed from signal handlers and exit function)

void sendCommandMessage (enum InputType messageType, int connectionID); //Send an input message to the controller

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
		otherError ("Cannot register SIGHUP handler for input program");
	if (sigaction (SIGTERM, &handleExit, NULL) < 0)
		otherError ("Cannot register SIGTERM handler for input program");

	if (argc != 2)
		argError ("Expected 1 argument", "Assignment1-Inputs controller-pid", "./Assignment1-Inputs 77235");

	pid_t controllerPID = parseNumArg (argv [1]);

	connectionID = ConnectAttach (ND_LOCAL_NODE, controllerPID, INPUT_CHANNEL_ID, _NTO_SIDE_CHANNEL, 0);
	if (connectionID < 0)
		otherError ("Couldn't attach message connection to controller");

	printf ("Available inputs: %s\n\nEnter: 'exit' to exit\n", AVAILABLE_INPUTS);

	char input [50] = {'\0'};
	unsigned int inputIndex;

	while (1)
	{
		puts ("");

		printf ("Select event: ");
		fflush (stdout);

		scanf ("%s", input);

		if (strcmp (input, "exit") == 0)
			exit (EXIT_SUCCESS);

		for (inputIndex = 0u; inputIndex < NUM_COMMANDS; ++inputIndex)
		{
			if (strcmp (input, COMMANDS [inputIndex]) == 0)
			{
				sendCommandMessage (inputTypes [inputIndex], connectionID);

				printf ("Sent message with command: %s\n", COMMANDS [inputIndex]);

				break;
			}
		}

		if (inputIndex == NUM_COMMANDS)
			puts ("Input did not match a command");
	}

	return EXIT_SUCCESS;
}

void sendCommandMessage (enum InputType messageType, int connectionID)
{
	InputMessage message;
	message.messageType = messageType;
	message.personID = 0u;
	message.weight = 0u;

	if (message.messageType == LEFT_SCAN || message.messageType == RIGHT_SCAN)
	{
		printf ("Please enter the person's ID number: ");
		fflush (stdout);

		scanf ("%u", &message.personID);
	}

	else if (message.messageType == WEIGHT_SCALE)
	{
		printf ("Please enter the person's weight: ");
		fflush (stdout);

		scanf ("%u", &message.weight);
	}

	int replyValue = 0; //Throwaway reply value
	if (MsgSend (connectionID, &message, sizeof (message), &replyValue, sizeof (replyValue)) < 0)
		otherError ("Couldn't send input message to controller");
}

void signalCleanup (int signal)
{
	ConnectDetach (connectionID);

	exit (0); //Not sure if necessary, do kill signals still kill the process if I override the default handler?
}

void exitCleanup (void)
{
	ConnectDetach (connectionID);
}
