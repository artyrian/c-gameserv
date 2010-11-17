#ifndef _SERVER_H_
#define _SERVER_H_

#include "main.h"

struct sockaddr_in;

int GetNumberPlayers (char **);
void PrintServerInfo (int, int);
void PrintDescriptors (int *, int);

void InitSockaddr (struct sockaddr_in *, char **);
void CreateListenSocket (int *);
void ListeningState (int );
void BindAddress (int , struct sockaddr_in *);
void CheckActionOnFD_SET (int, fd_set *);
void CheckDataFromClients (struct clientlist *, fd_set *);

#endif
