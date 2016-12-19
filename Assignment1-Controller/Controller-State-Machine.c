#include "Controller-State-Machine.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>

//Get the next message from the input program
InputMessage getNextInputMessage (int inputChannelID)
{
	if (inputChannelID < 0)
		otherError ("Channel ID is negative, did you check for errors?");

	InputMessage message;

	int receiveID = MsgReceive (inputChannelID, &message, sizeof (message), NULL);
	if (receiveID < 0)
		otherError ("Could not receive input message");

	//Reply with 0 (junk value)
	int replyValue = 0;
	if (MsgReply (receiveID, 0, &replyValue, sizeof (replyValue)) < 0)
		otherError ("Could not reply to input message");

	return message;
}

//Send a message to the display program
void sendOutputMessage (char * message, int outputConnectionID)
{
	if (message == NULL || outputConnectionID < 0)
		return;

	StatusMessage messageStruct;
	strcpy (messageStruct.message, message);

	int reply = 0; //Throwaway reply value
	if (MsgSend (outputConnectionID, &messageStruct, sizeof (messageStruct), &reply, sizeof (reply)) < 0)
		otherError ("Could not send output message");
}

StateHandler start (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("System starting\n", outputConnectionID);

	StateHandler returnHandler;

	returnHandler.handler = idle;

	return returnHandler;
}

StateHandler idle (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("System in idle state (waiting for left or right scan)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == LEFT_SCAN)
		returnHandler.handler = requestedLeftEntry;

	else if (message->messageType == RIGHT_SCAN)
		returnHandler.handler = requestedRightEntry;

	else
	{
		returnHandler.handler = idle;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler requestedLeftEntry (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	char outputBuffer [100] = {'\0'};
	sprintf (outputBuffer, "Left entry requested with ID: %u \n", message->personID);
	sendOutputMessage (outputBuffer, outputConnectionID);

	StateHandler returnHandler;
	returnHandler.handler = authenticatedLeft;

	return returnHandler;
}

StateHandler requestedRightEntry (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	char outputBuffer [100] = {'\0'};
	sprintf (outputBuffer, "Right entry requested with ID: %u\n", message->personID);
	sendOutputMessage (outputBuffer, outputConnectionID);

	StateHandler returnHandler;
	returnHandler.handler = authenticatedRight;

	return returnHandler;
}

StateHandler authenticatedLeft (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("User authenticated at left door (waiting for guard left unlock)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == GUARD_LEFT_UNLOCK)
		returnHandler.handler = unlockedLeft;

	else
	{
		returnHandler.handler = authenticatedLeft;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler authenticatedRight (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("User authenticated at right door (waiting for guard right unlock)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == GUARD_RIGHT_UNLOCK)
		returnHandler.handler = unlockedRight;

	else
	{
		returnHandler.handler = authenticatedRight;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler deniedEntry (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	StateHandler returnHandler;

	returnHandler.handler = NULL;

	return returnHandler;
}

StateHandler unlockedLeft (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("Left door unlocked (waiting for left open)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == LEFT_OPEN)
		returnHandler.handler = enteredLeft;

	else
	{
		returnHandler.handler = unlockedLeft;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler unlockedRight (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("Right door unlocked (waiting for right open)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == RIGHT_OPEN)
		returnHandler.handler = enteredRight;

	else
	{
		returnHandler.handler = unlockedRight;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler enteredLeft (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("User entered through left door (waiting for weight scale)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == WEIGHT_SCALE)
		returnHandler.handler = weighed;

	else
	{
		returnHandler.handler = enteredLeft;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler enteredRight (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("User entered through left door (waiting for weight scale)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == WEIGHT_SCALE)
		returnHandler.handler = weighed;

	else
	{
		returnHandler.handler = enteredLeft;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler weighed (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	char outputBuffer [100] = {'\0'};
	sprintf (outputBuffer, "User weighed with weight: %u (waiting for left or right close)\n", message->weight);
	sendOutputMessage (outputBuffer, outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == LEFT_CLOSE)
		returnHandler.handler = closedLeft;

	else if (message->messageType == RIGHT_CLOSE)
		returnHandler.handler = closedRight;

	else
	{
		returnHandler.handler = weighed;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler closedLeft (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("Left door closed (waiting for guard left lock)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == GUARD_LEFT_LOCK)
		returnHandler.handler = lockedLeft;

	else
	{
		returnHandler.handler = closedLeft;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler closedRight (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("Right door closed (waiting for guard right lock)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == GUARD_RIGHT_LOCK)
		returnHandler.handler = lockedRight;

	else
	{
		returnHandler.handler = closedRight;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler lockedLeft (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("Left door locked (waiting for guard right unlock)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == GUARD_RIGHT_UNLOCK)
		returnHandler.handler = requestedRightExit;

	else
	{
		returnHandler.handler = lockedLeft;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler lockedRight (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("Right door locked (waiting for guard left unlock)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == GUARD_LEFT_UNLOCK)
		returnHandler.handler = requestedLeftExit;

	else
	{
		returnHandler.handler = lockedRight;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler requestedLeftExit (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("Left exit requested (waiting for left open)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == LEFT_OPEN)
		returnHandler.handler = openedLeftExit;

	else
	{
		returnHandler.handler = requestedLeftExit;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler requestedRightExit (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("Right exit requested (waiting for right open)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == RIGHT_OPEN)
		returnHandler.handler = openedRightExit;

	else
	{
		returnHandler.handler = requestedRightExit;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler openedLeftExit (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("Left door opened and user has exited (waiting for left close)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == LEFT_CLOSE)
		returnHandler.handler = closedLeftExit;

	else
	{
		returnHandler.handler = openedLeftExit;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler openedRightExit (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("Right door opened and user has exited (waiting for right close)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == RIGHT_CLOSE)
		returnHandler.handler = closedRightExit;

	else
	{
		returnHandler.handler = openedRightExit;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler closedLeftExit (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("Left door closed (waiting for guard left lock)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == GUARD_LEFT_LOCK)
		returnHandler.handler = lockedLeftExit;

	else
	{
		returnHandler.handler = closedLeftExit;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler closedRightExit (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("Right door closed (waiting for guard right lock)\n", outputConnectionID);

	*message = getNextInputMessage (inputChannelID);

	StateHandler returnHandler;

	if (message->messageType == GUARD_RIGHT_LOCK)
		returnHandler.handler = lockedRightExit;

	else
	{
		returnHandler.handler = closedRightExit;

		sendOutputMessage ("Unexpected input, resetting this state\n", outputConnectionID);
	}

	return returnHandler;
}

StateHandler lockedLeftExit (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("Left door locked\n", outputConnectionID);

	StateHandler returnHandler;

	returnHandler.handler = idle;

	return returnHandler;
}

StateHandler lockedRightExit (InputMessage * message, int inputChannelID, int outputConnectionID)
{
	if (message == NULL || inputChannelID < 0 || outputConnectionID < 0)
		otherError ("Invalid parameters in state handler");

	sendOutputMessage ("Right door locked\n", outputConnectionID);

	StateHandler returnHandler;

	returnHandler.handler = idle;

	return returnHandler;
}
