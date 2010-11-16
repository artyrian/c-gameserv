#include "client.h"



/* */
struct client * GetPrewLastClient(struct clientlist *clList)
{
	struct client * user, * returnValueUser;
	user = (struct client *) malloc (sizeof(struct client));

	returnValueUser = clList->first;
	user = clList->first;

	while ( user != NULL )
	{
		if ( user->next != NULL)
			returnValueUser = user;
		else
			break;
		user = user->next;	
	}
	free (user);
	return returnValueUser;

}



/* */
void ConnectClient (struct clientlist *clList, int fd)
{
	const char strConnected[30] = "You connected to server.\n";

	struct client * new_user;
	
	new_user = (struct client *) malloc (sizeof(struct client));

	if ( clList->first == NULL )
		clList->first = new_user;
	else
		clList->last->next = new_user;

	clList->last = new_user;
	clList->cnt ++;
	
	new_user->fd = fd;
	new_user->num = clList->cnt;
	new_user->next = NULL;

	printf ("New client connected. FD=%d. #%d.\n", fd, clList->cnt);

	write ( fd, strConnected, strlen(strConnected) + 1 );
}



/* */
void DisconnectClient (struct clientlist *clList, struct client *user)
{
	int fd = user->fd; 

	shutdown(fd, 2);
	close (fd);
	printf ("Client disconnected. FD=%d.\n", fd);


	user->fd = clList->last->fd;
	user->num = clList->last->num;
	
	free(clList->last);
	clList->last = GetPrewLastClient (clList);
	clList->last->next = NULL;
	clList->cnt --;
	printf ("Clients now:%d.\n", clList->cnt);

	if ( clList->cnt  == 0)
	{
		clList->first = clList->last = NULL;
		printf ("All players disconnected :-(\n");
	}
}



/* */
void DeniedClient (int fd)
{
	const char strFullServer[30] = "Sorry! The server is full.\n";
	write (	fd, strFullServer, strlen(strFullServer) + 1);
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
