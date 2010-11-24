#include "print.h"



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
void PrintClientlist (struct clientlist *clList)
{
	struct client *user;

	user = (struct client *) malloc (sizeof(struct client));
	user = clList->first;

	printf ("Print clientlist:\n");
	while ( user != NULL )
	{
		printf ("FD=%d. #%d. ", user->fd, user->num);
		user = user -> next;
	}
	printf ("End of clientlist.\n");
	free (user);
}



/* */
void PrintToFDSuccessConnect (int fd)
{
	char * strConnected;
	strConnected = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strConnected, "You're connected to server.\n");
	write ( fd, strConnected, strlen(strConnected) + 1 );
	free (strConnected);
}



/* */
void PrintToAll(struct clientlist * clList, char * strInfo)
{
	struct client * user;
	int fd;

	user = clList->first;

	while ( user != NULL )
	{
		fd = user->fd;
		write (fd, strInfo, strlen(strInfo) + 1);
		user = user->next;
	}
}
