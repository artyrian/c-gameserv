#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "main.h"
#include "game.h"

#define NUMBER_OF_COMMANDS 10

struct word
{
	char * str;
	struct word * next;
};

struct command
{
	int cnt;
	struct word * first;
	struct word * last;
};

void InitCommand (struct command *);
void ClearCommand (struct command *);

void Market (struct banker *);
void PlayerInfo (struct banker *);
void Production (struct banker *);
void Buy (struct banker *);
void Sell (struct banker *);
void Build (struct banker *);
void TurnStep (struct banker *);
void WhoAmI (struct banker *);
void Help (int);

void PrintHelp (struct banker *);
#endif

