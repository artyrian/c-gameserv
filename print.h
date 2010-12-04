#ifndef _PRINT_H_
#define _PRINT_H_

#include "main.h"
#include "client.h"
#include "auction.h"


#define MESSAGE_LENGHT 64


void PrintWrongPriceBuy (struct banker *);
void PrintWrongItemBuy (struct banker *);
void PrintWrongPriceSell (struct banker *);
void PrintWrongItemSell (struct banker *);
void PrintCantCreateProd (struct banker *);
void PrintWillCreateProd (struct banker *);
void PrintBankrupt (struct client *);
void PrintMadeProd (struct client *, int i);
void PrintGameNotStarted (int);
void PrintServerInfo (int, int);
void PrintDescriptors (int *, int);


void PrintToFDSuccessConnect (int);
void PrintToAll (struct clientlist *, char *);
void PrintSuccessBuy (struct clientlist *);
void PrintSuccessSell (struct clientlist *);
void PrintSuccessBuild (struct clientlist *);
void PrintTurnOn (struct clientlist *);
void PrintComing (int, int);

#endif
