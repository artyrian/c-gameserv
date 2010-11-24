#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "main.h"
#include "print.h"

#define NUMBER_OF_COMMANDS 10

struct word
{
	char * str;
	struct word * next;
};

struct command
{
	struct word * first;
	struct word * last;
};

void InitCommand (struct command *);
void Help (int);

void IncrementVar ();
void PrintVar (int);
void PrintHelp (int);
void GameNotStarted (int);

#endif

