#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "main.h"

struct client
{
	int fd;
	int num;
	struct buffer *buf;
	struct client *next;
};

struct clientlist
{
	int cnt;
	struct client *first;
	struct client *last;
};

void ConnectClient (struct clientlist *, int);
void DisconnectClient (struct clientlist *, struct client *);
void DeniedClient (int);

void PrintClientlist (struct clientlist *);
#endif
