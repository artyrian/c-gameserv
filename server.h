#ifndef _SERVER_H_
#define _SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <netinet/ip.h>

#include "client.h"
#include "game.h"
#include "main.h"

struct settings
{
	int fd;
	int num;
};


struct sockaddr_in;

void InitSockaddr (struct sockaddr_in *, char **);
void CreateListenSocket (int *);
void BindAddress (int , struct sockaddr_in *);
void ListeningState (int );

void InitSettings (struct settings *, int, int);

void CheckActionOnFD_SET (struct clientlist *, int, fd_set *);
void CheckDataFromClients (struct banker *, fd_set *);

int GetNumberPlayers (char **);

#endif
