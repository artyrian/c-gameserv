#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "main.h"
#include "buffer.h"
#include "game.h"

#define MESSAGE_LENGHT 64
struct client
{
	int fd;
	int num;
	int fExit;
	struct buffer *buf;
	struct command *cmd;
	struct client *next;
};

struct clientlist
{
	int maxPlayers;
	int cnt;
	int statusStartGame;	
	struct client *first;
	struct client *current;
	struct client *last;
};

void CreateClientList (struct clientlist **, char **);

int MaxDescriptor (struct clientlist *, fd_set *, int);

void AcceptQuery (int, struct clientlist *);

void ConnectClient (struct clientlist *, int);
void DisconnectClient (struct clientlist *);
void DeniedClient (int);

int ReadToBuffer (struct client *, int);

char * StatusUsersConnecting (struct clientlist *);

#endif
