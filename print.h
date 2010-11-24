#ifndef _PRINT_H_
#define _PRINT_H_

#include "main.h"
#include "client.h"




void PrintServerInfo (int, int);
void PrintDescriptors (int *, int);

void PrintClientlist (struct clientlist *);

void PrintToFDSuccessConnect (int);
void PrintToAll (struct clientlist *, char *);

#endif
