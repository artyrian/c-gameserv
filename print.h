#ifndef _PRINT_H_
#define _PRINT_H_

#include "main.h"
#include "client.h"
#include "game.h"



void PrintRandomArray (int *, int);
void PrintListBuy (struct listAuction *);
void PrintListSell (struct listAuction *);
void PrintServerInfo (int, int);
void PrintDescriptors (int *, int);

void PrintClientlist (struct clientlist *);

void PrintToFDSuccessConnect (int);
void PrintToAll (struct clientlist *, char *);
void PrintSuccessBuy (struct clientlist *);
void PrintSuccessSell (struct clientlist *);
void PrintSuccessBuild (struct clientlist *);
void PrintTurnOn (struct clientlist *);
void PrintComing (int, int);

#endif
