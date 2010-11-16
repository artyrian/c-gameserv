#ifndef _SERVERINFO_H_
#define _SERVERINFO_H_

#include "main.h"

struct sockaddr_in;

int getNumberPlayers (char **);
int getPort (char **);
void printServerInfo (int, int);
void printDescriptors (int *, int);


#endif
