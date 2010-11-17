#ifndef _SERVER_H_
#define _SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <netinet/ip.h>

#include "client.h"
#include "main.h"

struct sockaddr_in;

int GetNumberPlayers (char **);
void PrintServerInfo (int, int);
void PrintDescriptors (int *, int);

void InitSockaddr (struct sockaddr_in *, char **);
void CreateListenSocket (int *);
void ListeningState (int );
void BindAddress (int , struct sockaddr_in *);
void CheckActionOnFD_SET (struct clientlist *, int, fd_set *);
void CheckDataFromClients (struct clientlist *, fd_set *);

#endif
