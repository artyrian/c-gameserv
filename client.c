#include "client.h"



/* */
void InitUser (struct client * user, struct clientlist *clList, int fd)
{
	user->fd = fd;
	user->num = clList->cnt;
	user->next = NULL;
	user->buf = (struct buffer*) malloc (sizeof(struct buffer));

	InitBuffer ( user->buf );
}




/* */
void CreateClientList (struct clientlist ** clList, char ** argv)
{
	(*clList) = (struct clientlist *) malloc(sizeof(struct clientlist));
	(*clList)->first = (*clList)->last = NULL;
	(*clList)->maxPlayers = GetNumberPlayers(argv);
}



/* */
int MaxDescriptor (struct clientlist *clList, fd_set *readfds, int ls)
{
	int max_d;
	int fd;
	struct client * user;

	max_d = ls;
	user = clList->first;

	while ( user != NULL )
	{
		fd = user->fd;
		FD_SET (fd, readfds);
		if (fd > max_d)
			max_d = fd;
		user = user->next;
	}

	return max_d;
}



/* */
void AcceptQuery (int ls, struct clientlist * clList)
{
	int fd;

	fd = accept (ls, NULL, NULL); 

	if ( clList->cnt < clList->maxPlayers)
		ConnectClient (clList, fd);
	else
		DeniedClient (fd);
}



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

	struct client * user;
	
	user = (struct client *) malloc (sizeof(struct client));

	if ( clList->first == NULL )
		clList->first = user;
	else
		clList->last->next = user;

	clList->last = user;
	clList->cnt ++;

	InitUser (user, clList, fd);

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
	
	free(clList->last->buf->str);
	free(clList->last->buf);
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
int ReadToBuffer (struct client * user, int fd)
{
	int rc;
	char c; 
	char * str;
	
	str = user->buf->str;

	rc = read (fd, &c, sizeof(char));
	if ( rc == -1 )
	{
		perror ("read");
	}
	
	if ( rc != 0 )
	{
		str[user->buf->cnt++] = c;
		str[user->buf->cnt] = '\0';
	}

	if ( user->buf->cnt  == BUF_SIZE * user->buf->part - 1)
		ExtendBuffer (user->buf);

	return rc;
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
