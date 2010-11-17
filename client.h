#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "main.h"
#include "buffer.h"

struct client
{
	int fd;
	int num;
	struct buffer *buf;
	struct client *next;
};

struct clientlist
{
	int maxPlayers;
	int cnt;
	struct client *first;
	struct client *last;
};

void CreateClientList (struct clientlist **, char **);

int MaxDescriptor (struct clientlist *, fd_set *, int);

void AcceptQuery (int, struct clientlist *);

void ConnectClient (struct clientlist *, int);
void DisconnectClient (struct clientlist *, struct client *);
void DeniedClient (int);

int ReadToBuffer (struct client *, int);

void PrintClientlist (struct clientlist *);
#endif
