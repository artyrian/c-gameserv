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

int getNumberPlayers (char **);
int getPort (char **);
void printServerInfo (int, int);
void printDescriptors (int *arr_d, int cur_num);

int main(int argc, char ** argv, char ** envp)
{
	int numberPlayers = getNumberPlayers (argv);
	int port = getPort (argv);
	
	int ls = socket(AF_INET, SOCK_STREAM, 0);
	
	int *arr_d = (int *) malloc (numberPlayers*sizeof (int));
	int i, cur_num = 0;
	
	struct sockaddr_in addr;
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (-1 == ls)
	{
		perror ("socket");
		exit (1);
	}

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

		char buf[32];
		int rc;


		i = 0;
		//for (fd = arr_d[i]; i < cur_num; fd = arr_d[i++])
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
			printf ("New client connecting...\n");
			arr_d[cur_num++] = fd = accept (ls, NULL, NULL); 
			printf ("cur_num=%d, fd = %d\n", cur_num, fd);
		}
		
		printDescriptors (arr_d, cur_num);
		
		i = 0;

		//for (fd = arr_d[i]; i < cur_num; fd = arr_d[i++])arr_d
		for (i = 0; i < cur_num; i++)
		{
			fd = arr_d[i];
			printf ("Start cycle wait data\n");
			if (FD_ISSET(fd, &readfds))
			{
				printf ("Start reading.\n");

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
				}
				else 
				{
					printf ("Must be shutdown\n");
					if ( cur_num > 1 )
						arr_d[i] = arr_d[cur_num];
						cur_num--;
				}
				printf ("End read cycle\n");
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

	printf ("List of descriptors:\n");
	for ( i = 0; i < cur_num; i++ ) 
		printf ("fd=%d\n", arr_d[i]);
}
