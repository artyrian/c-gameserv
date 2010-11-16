#include "main.h"

int main(int argc, char ** argv, char ** envp)
{
	
	int numberPlayers = getNumberPlayers (argv);
	int port =  getPort (argv);
	
	int ls = socket(AF_INET, SOCK_STREAM, 0);
	
	struct clientlist *clList;
	clList = (struct clientlist *) malloc(sizeof(struct clientlist));
	clList->first = clList->last = NULL;
	
	struct client *user;
	user= (struct client *) malloc (sizeof(struct client));

	struct sockaddr_in addr;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (-1 == ls)
	{
		perror ("socket");
		exit (1);
	}

	int opt = 1;
	setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	if (0 != bind(ls, (struct sockaddr *) &addr, sizeof(addr)))
	{
		perror("bind");
		exit (1);
	}

	if (-1 == listen(ls, 5)) 
	{
		perror ("listen");
		exit (1);
	} 

	printServerInfo (port, numberPlayers);

	for (;;)
	{ 
		fd_set readfds; 
		int max_d = ls; 

		FD_ZERO (&readfds);
		FD_SET (ls, &readfds);

		int fd;

		char buf[BUF_SIZE];
		int rc;
		

		user = clList->first;
		while ( user != NULL )
		{
			fd = user->fd;
			FD_SET (fd, &readfds);
			if (fd > max_d)
				max_d = fd;
			user = user->next;
		}


		int res = select (max_d + 1, &readfds, NULL, NULL, NULL);
		if (res < 1)
		{
			perror ("select\n");
			exit (1);
		}

		if (FD_ISSET(ls, &readfds))
		{
			fd = accept (ls, NULL, NULL); 
			if ( clList->cnt < numberPlayers )
			 	ConnectClient (clList, fd);
			else
				DeniedClient (fd);
		}
		
		user = clList->first;
		while ( user != NULL )
		{
			fd = user->fd;
			if (FD_ISSET(fd, &readfds))
			{
				rc = read (fd, buf, sizeof(buf)-1); 
				if ( rc == -1 )
				{
					perror ("read");
					break;
				}
				if ( rc > 0 )
				{
					buf[rc] = '\0';
					printf ("%s\n", buf);
					if ( buf[rc - 1] == '\n' )
						printf ("ENTER!\n");
				}
				else 
					DisconnectClient(clList, user);
			}
			user = user->next;
		}

	}

	free (user);
	free (clList);
	return 0;
}


