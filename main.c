#include <stdio.h>	//-
#include <sys/types.h>	//+
#include <sys/socket.h>	//+
#include <sys/times.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define BUF_SIZE 128

int getNumberPlayers (char **);
int getPort (char **);
void printServerInfo (int, int);
void printDescriptors (int *, int);
void DisconnectClient (int, int **, int *);

int main(int argc, char ** argv, char ** envp)
{
	int numberPlayers = getNumberPlayers (argv);
	int port = getPort (argv);
	
	int ls = socket(AF_INET, SOCK_STREAM, 0);
	
	int *arr_d = (int *) malloc (numberPlayers*sizeof (int));
	int i, cur_num = 0;

	const char connectedMsg[30] = "You connected to server.\n";
	
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


		for (i = 0; i < cur_num; i++)
		{
			fd = arr_d[i];
			FD_SET (fd, &readfds);
			if (fd > max_d)
				max_d = fd;
		}



		int res = select (max_d + 1, &readfds, NULL, NULL, NULL);
		if (res < 1)
		{
			perror ("select\n");
			exit (1);
		}

		if (FD_ISSET(ls, &readfds))
		{
			arr_d[cur_num++] = fd = accept (ls, NULL, NULL); 
			printf ("New client connected. FD=%d. #%d.\n", fd, cur_num);
			write (fd, connectedMsg, strlen(connectedMsg) + 1 );
		}
		
		for (i = 0; i < cur_num; i++)
		{
			fd = arr_d[i];
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
					DisconnectClient(i, &arr_d, &cur_num);
			}
		}
	}

	return 0;
}



/* get number of players from argv*/
int getNumberPlayers  (char ** argv)
{
	int num = 0;
	
	if ( argv[1] != '\0' )
		num = atoi (argv[1]);
	return num;
}



/* */
int getPort (char ** argv)
{
	int port = 0;

	if ( argv[2] != '\0' )
		port = atoi (argv[2]);
	return port;
}



/* */
void printServerInfo (int port, int numPl)
{
	printf ("Max. players:%d\n", numPl);
	printf ("Port:%d.\n", port);
	printf ("IP adress:(Isn't ready).\n");
}



/* */
void printDescriptors (int *arr_d, int cur_num)
{
	int i;
	
	printf ("Current number of players:%d\n", cur_num);
	printf ("List of descriptors:\n");
	for ( i = 0; i < cur_num; i++ ) 
		printf ("fd=%d.", arr_d[i]);
	printf ("\n");
}



/* */
void DisconnectClient (int i, int ** arr_d, int * n)
{
	int fd = (*arr_d)[i];

	printf ("Client disconnected. FD=%d.\n", fd);
	shutdown(fd, 2);
	close (fd);
	(*arr_d)[i] = (*arr_d)[--(*n)];
	if ( (*n) == 0)
		printf ("All players disconnected :-(\n");
}
