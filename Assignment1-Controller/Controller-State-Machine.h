#pragma once

#ifndef CONTROLLER_STATE_MACHINE_H_
#define CONTROLLER_STATE_MACHINE_H_

#include "Assignment1-Common.h"

InputMessage getNextInputMessage (int inputChannelID);
void sendOutputMessage (char * message, int outputConnectionID);

typedef struct StateHandlerStruct
{
	struct StateHandlerStruct (*handler) (InputMessage * message, int inputChannelID, int outputConnectionID);
} StateHandler;

StateHandler start (InputMessage * message, int inputChannelID, int outputConnectionID);

StateHandler idle (InputMessage * message, int inputChannelID, int outputConnectionID);

StateHandler requestedLeftEntry (InputMessage * message, int inputChannelID, int outputConnectionID);
StateHandler requestedRightEntry (InputMessage * message, int inputChannelID, int outputConnectionID);

StateHandler authenticatedLeft (InputMessage * message, int inputChannelID, int outputConnectionID);
StateHandler authenticatedRight (InputMessage * message, int inputChannelID, int outputConnectionID);

StateHandler deniedEntry (InputMessage * message, int inputChannelID, int outputConnectionID);

StateHandler unlockedLeft (InputMessage * message, int inputChannelID, int outputConnectionID);
StateHandler unlockedRight (InputMessage * message, int inputChannelID, int outputConnectionID);

StateHandler enteredLeft (InputMessage * message, int inputChannelID, int outputConnectionID);
StateHandler enteredRight (InputMessage * message, int inputChannelID, int outputConnectionID);

StateHandler weighed (InputMessage * message, int inputChannelID, int outputConnectionID);

StateHandler closedLeft (InputMessage * message, int inputChannelID, int outputConnectionID);
StateHandler closedRight (InputMessage * message, int inputChannelID, int outputConnectionID);

StateHandler lockedLeft (InputMessage * message, int inputChannelID, int outputConnectionID);
StateHandler lockedRight (InputMessage * message, int inputChannelID, int outputConnectionID);

StateHandler requestedLeftExit (InputMessage * message, int inputChannelID, int outputConnectionID);
StateHandler requestedRightExit (InputMessage * message, int inputChannelID, int outputConnectionID);

StateHandler openedLeftExit (InputMessage * message, int inputChannelID, int outputConnectionID);
StateHandler openedRightExit (InputMessage * message, int inputChannelID, int outputConnectionID);

StateHandler closedLeftExit (InputMessage * message, int inputChannelID, int outputConnectionID);
StateHandler closedRightExit (InputMessage * message, int inputChannelID, int outputConnectionID);

StateHandler lockedLeftExit (InputMessage * message, int inputChannelID, int outputConnectionID);
StateHandler lockedRightExit (InputMessage * message, int inputChannelID, int outputConnectionID);

#endif
