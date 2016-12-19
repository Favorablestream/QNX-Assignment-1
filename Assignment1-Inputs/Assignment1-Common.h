#pragma once

#ifndef ASSIGNMENT1_COMMON_H_
#define ASSIGNMENT1_COMMON_H_

#include <stdlib.h>
#include <limits.h>

#define DISPLAY_CHANNEL_ID 1
#define INPUT_CHANNEL_ID 1

#define NUM_COMMANDS 12
enum InputType {LEFT_SCAN, RIGHT_SCAN, LEFT_OPEN, RIGHT_OPEN, LEFT_CLOSE, RIGHT_CLOSE, GUARD_LEFT_UNLOCK, GUARD_RIGHT_UNLOCK, GUARD_LEFT_LOCK, GUARD_RIGHT_LOCK, WEIGHT_SCALE, USER_EXIT};
extern enum InputType inputTypes [];

extern char * COMMANDS [];
extern char * AVAILABLE_INPUTS;

typedef struct InputMessage
{
	enum InputType messageType;
	unsigned int personID;
	unsigned int weight;
} InputMessage;

typedef struct StatusMessage
{
	char message [128];
} StatusMessage;

int parseNumArg (char * argument);

void argError (char * message, char * usage, char * example);
void otherError (char * message);

#endif
