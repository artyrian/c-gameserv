#include "server.h"
#include "parse.h"



/* get number of players from argv*/
int GetNumberPlayers  (char ** argv)
{
	int num = 0;
	
	if ( argv[1] != '\0' )
		num = atoi (argv[1]);

//	num = 3;

	return num;
}



/* */
int GetPort (char ** argv)
{
	int port = 0;

	if ( argv[2] != '\0' )
		port = atoi (argv[2]);

//	port = 7777;

	return port;
}




/* */
void InitSockaddr (struct sockaddr_in * addr, char ** argv)
{
	(*addr).sin_family = AF_INET;
	(*addr).sin_port = htons(GetPort(argv));
	(*addr).sin_addr.s_addr = INADDR_ANY;
}



/* */
void CreateListenSocket (int * ls)
{
	*ls = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == *ls)
	{
		perror ("socket");
		exit (1);
	}
}



/* */
void ListeningState (int ls)
{
	if (-1 == listen(ls, 5)) 
	{
		perror ("listen");
		exit (1);
	}
}



/* */
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


/* */
void CheckActionOnFD_SET (struct clientlist *clList,
				int ls,
				fd_set *readfds)
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



/* */
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
				ParseCommand (bank);
			if ( rc == 0 )
				DisconnectClient(clList);
		}
		clList->current = clList->current->next;
	}
}

