#include "server.h"
#include "parse.h"
#include <stdlib.h>
#include <stdio.h>


int GetNumberPlayers  (char **);
int GetPort (char **);
void InitSettings (struct settings *, int, int);
void InitSockaddr (struct sockaddr_in *, char **);
void CreateListenSocket (int *);
void ListeningState (int);
void BindAddress (int, struct sockaddr_in *);
void CheckActionOnFD_SET (struct clientlist *, int, fd_set *);
void CheckDataFromClients (struct banker *, fd_set *);



/* get number of players from argv*/
int GetNumberPlayers  (char ** argv)
{
	int num = 0;
	
	if ( argv[1] != '\0' )
	{
		num = atoi (argv[1]);
	}

	num = 2;
	return num;
}



/* Get number of port from argv */
int GetPort (char ** argv)
{
	int port = 0;

	if ( argv[2] != '\0' )
	{
		port = atoi (argv[2]);
	}

	port = 7777;
	return port;
}



/* Initializating settings information
 */
void InitSettings (struct settings * contact, int fd, int cnt) 
{
	contact->fd = fd;
	contact->num = cnt;
}



/*
 */
void InitSockaddr (struct sockaddr_in * addr, char ** argv)
{
	(*addr).sin_family = AF_INET;
	(*addr).sin_port = htons(GetPort(argv));
	(*addr).sin_addr.s_addr = INADDR_ANY;
}



/*
 */
void CreateListenSocket (int * ls)
{
	*ls = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == *ls)
	{
		perror ("socket");
		exit (1);
	}
}



/*
 */
void ListeningState (int ls)
{
	if (-1 == listen(ls, 5)) 
	{
		perror ("listen");
		exit (1);
	}
}



/* 
 */
void BindAddress (int ls, struct sockaddr_in * addr)
{
	int opt = 1;

	setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (0 != bind(ls, (struct sockaddr *) addr, sizeof(*addr)))
	{
		perror("bind");
		exit (1);
	}
}



/*
 */
void CheckActionOnFD_SET (struct clientlist * clList, int ls,
				fd_set * readfds)
{
	int res;
	int max_d;

	max_d = MaxDescriptor (clList, readfds, ls);

	res = select (max_d + 1, readfds, NULL, NULL, NULL);
	if (res < 1)
	{
		perror ("select\n");
		exit (1);
	}
}



/*
 */
void CheckDataFromClients (struct banker * bank, fd_set * readfds)
{
	struct clientlist * clList;
	int fd;
	int rc;

	clList = bank->clList;
	clList->current = clList->first;

	while ( clList->current != NULL )
	{
		fd = clList->current->contact->fd;
		if (FD_ISSET(fd, readfds))
		{
			rc = ReadToBuffer (clList->current, fd); 
			if ( rc > 0 )
			{
				ParseCommand (bank);
			}
			else // if rc == 0 (because to (-1) check in RtoB()
			{
				DisconnectClient(clList);
			}
		}
		clList->current = clList->current->next;
	}
}
