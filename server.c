#include "server.h"
#include "parse.h"



/* get number of players from argv*/
int GetNumberPlayers  (char ** argv)
{
	int num = 0;
	
	if ( argv[1] != '\0' )
		num = atoi (argv[1]);
	return num;
}



/* */
int GetPort (char ** argv)
{
	int port = 0;

	if ( argv[2] != '\0' )
		port = atoi (argv[2]);
	return port;
}



/* */
void PrintServerInfo (int port, int numPl)
{
	printf ("Max. players:%d\n", numPl);
	printf ("Port:%d.\n", port);
}



/* */
void PrintDescriptors (int *arr_d, int cur_num)
{
	int i;
	
	printf ("Current number of players:%d\n", cur_num);
	printf ("List of descriptors:\n");
	for ( i = 0; i < cur_num; i++ ) 
		printf ("fd=%d.", arr_d[i]);
	printf ("\n");
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
void CheckDataFromClients (struct clientlist * clList, fd_set * readfds)
{
	struct client * user;
	int fd;
	int rc;

	user = (struct client *) malloc (sizeof(struct client));
	user = clList->first;

	while ( user != NULL )
	{
		fd = user->fd;
		if (FD_ISSET(fd, readfds))
		{
			rc = ReadToBuffer (user, fd); 
			if ( rc > 0 )
				ParseCommand (user->buf, rc);
			else 
				DisconnectClient(clList, user);
		}
		user = user->next;
	}

	free (user);
}

