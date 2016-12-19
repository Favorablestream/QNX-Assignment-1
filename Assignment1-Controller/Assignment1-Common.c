#include "Assignment1-Common.h"

#include <stdio.h>
#include <stdlib.h>

char * COMMANDS [] = {"ls", "rs", "lo", "ro", "lc", "rc", "glu", "gru", "gll", "grl", "ws", "ue"};
char * AVAILABLE_INPUTS = "ls (left scan), rs (right scan), ws (weight scale), lo (left open), ro (right open), lc (left closed),\nrc (right closed), glu (guard left unlock), gru (guard right unlock), gll (guard left lock), grl (guard right lock), ue (user exit)";

enum InputType inputTypes [] = {LEFT_SCAN, RIGHT_SCAN, LEFT_OPEN, RIGHT_OPEN, LEFT_CLOSE, RIGHT_CLOSE, GUARD_LEFT_UNLOCK, GUARD_RIGHT_UNLOCK, GUARD_LEFT_LOCK, GUARD_RIGHT_LOCK, WEIGHT_SCALE};

int parseNumArg (char * argument)
{
	if (argument == NULL)
		exit (EXIT_FAILURE);

	char * end;

	long int input = strtol (argument, &end, 10);

	if (*end != '\0')
		otherError ("Could not parse argument as an integer");

	if (input < INT_MIN || input > INT_MAX)
		otherError ("Integer input would overflow the type (int)");

	return (int) input;
}

void argError (char * message, char * usage, char * example)
{
	if (message == NULL || usage == NULL || example == NULL)
		exit (EXIT_FAILURE);

	perror (message);
	puts ("");

	printf ("Usage: %s\n", usage);
	printf ("Example: %s\n", example);

	exit (EXIT_FAILURE);
}

void otherError (char * message)
{
	if (message == NULL)
		exit (EXIT_FAILURE);

	perror (message);

	exit (EXIT_FAILURE);
}
