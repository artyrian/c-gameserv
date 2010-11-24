#ifndef _GAME_H_
#define _GAME_H_

#include "main.h"
#include "client.h"


struct banker
{
	struct clientlist * clList;
};


void CreateBank (struct banker **, char **);

#endif
