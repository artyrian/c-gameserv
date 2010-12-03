#include "client.h"
#include "server.h"
#include "print.h"
#include "commands.h"



/* */
void InitFlags (struct userFlags * f)
{
	f->turn = 0;
}



/* */
void InitUser (struct client * user, struct clientlist *clList, int fd)
{
	//user->number = clList->cnt; It's not need.fn better GetUserId()
	user->next = NULL;

	user->contact = (struct settings*)malloc(sizeof(struct settings));
	user->contact->fd = fd;
	user->contact->num = clList->cnt;

	user->f = (struct userFlags *) malloc (sizeof(struct userFlags));
	InitFlags (user->f);

	user->sell = (struct auction *) malloc (sizeof(struct auction));
	InitBuyOrSell (user->sell);

	user->buy = (struct auction *) malloc (sizeof(struct auction));
	InitBuyOrSell (user->buy);

	user->data = (struct stuff *) malloc (sizeof(struct stuff));
	InitStuff (user->data);

	user->buf = (struct buffer * ) malloc (sizeof(struct buffer));
	InitBuffer (user->buf);

	user->cmd = (struct command * ) malloc (sizeof(struct command));
	InitCommand (user->cmd);
	
}




/* */
void CreateClientList (struct clientlist ** clList, char ** argv)
{
	(*clList) = (struct clientlist *) malloc(sizeof(struct clientlist));
	(*clList)->first = (*clList)->last = (*clList)->current = NULL;
	(*clList)->maxPlayers = GetNumberPlayers(argv);
	(*clList)->statusStartGame = 0;
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
		fd = user->contact->fd;
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

	if ( clList->cnt < clList->maxPlayers && !clList->statusStartGame) 
	{
		ConnectClient (clList, fd); 
	}
	else
	{
		DeniedClient (fd);
	}
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
	struct client * user;
	
	user = (struct client *) malloc (sizeof(struct client));

	if ( clList->first == NULL )
	{
		clList->first = user;
	}
	else
	{
		clList->last->next = user;
	}
	clList->last = user;

	clList->cnt ++;

	InitUser (user, clList, fd);

	printf ("New client connected. FD=%d. #%d.\n", fd, clList->cnt);

	PrintToFDSuccessConnect (fd);
	PrintToAll(clList, StatusUsersConnecting(clList));
	
	if ( clList->cnt == clList->maxPlayers )
	{
		GiveUserId (clList);
		clList->statusStartGame = 1;
		PrintToAll (clList, "The game start now.\n>= = = 1th month. = = =<\n>");

	}
}	

void CopyStructuresClient (struct clientlist *clList)
{
	clList->current->number = clList->last->number;

	clList->current->contact->fd = clList->last->contact->fd;
	clList->current->contact->num = clList->last->contact->num;

	clList->current->data->money = clList->last->data->money;
	clList->current->data->raw = clList->last->data->raw;
	clList->current->data->order = clList->last->data->order; 
	clList->current->data->product = clList->last->data->product;
	clList->current->data->factory = clList->last->data->factory;
	clList->current->data->cntBuild = clList->last->data->cntBuild;

	clList->current->data->project =  clList->last->data->project;

	clList->current->buy->item = clList->last->buy->item;
	clList->current->buy->price = clList->last->buy->price;
	clList->current->sell->item = clList->last->sell->item;
	clList->current->sell->price = clList->last->sell->price;

	clList->current->f->turn = clList->last->f->turn;
}



/* */
void DisconnectClient (struct clientlist *clList)
{
	int fd;
	
	fd = clList->current->contact->fd; 
	CopyStructuresClient (clList); // better to change this.

	shutdown(fd, 2);
	close (fd);
	printf ("Client disconnected. FD=%d.\n", fd);

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
	else
	{
		PrintToAll (clList, StatusUsersConnecting (clList));
	}

}



/* */
void DeniedClient (int fd)
{
	const char strFullServer[60] = "Sorry! The server is full or game started.\n";
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



/* Fn to create string status connected clients at the moment.
 * return string.
 */
char * StatusUsersConnecting (struct clientlist * clList)
{
	char * strInfo;
	int maxPlayers;
	int curPlayers;	
	
	strInfo = (char *) malloc (MESSAGE_LENGHT * sizeof(char));

	maxPlayers = clList->maxPlayers;
	curPlayers = clList->cnt;
	
	sprintf (strInfo, 
		"Now connected / Max Players:\n$\t%d\t\t%d\n", 
		curPlayers, maxPlayers);
	
	return strInfo;
}



/* Fn create string of players still active in game.
 * retur string
 */
char * StatusUsersPlaying (struct clientlist * clList)
{
	char * strInfo;
	int curPlayers;	
	
	strInfo = (char *) malloc (MESSAGE_LENGHT * sizeof(char));

	curPlayers = clList->cnt;
	
	sprintf (strInfo, 
		"Players still active:\n$\t%d\n", curPlayers);
	
	return strInfo;
}



/* */
char * GetInfoPlayer (struct client * user)
{
	const int STR_INFO = 5;
	char * strInfo;
	strInfo = (char *) malloc (MESSAGE_LENGHT * STR_INFO);
	
	sprintf (strInfo, 
		"# %d\tMONEY | RAW | PRODUCT | FACTORY | BUILDING\n$\t%d\t%d\t%d\t%d\t\t%d\n",
		user->number,
		user->data->money,
		user->data->raw,
		user->data->product,
		user->data->factory,
		user->data->cntBuild	
		);
	return strInfo;
}



/* */
void GiveUserId (struct clientlist * clList)
{
	int i;
	int fd;
	struct client * user;
	char * strInfo;

	strInfo = (char *) malloc (MESSAGE_LENGHT);
	user = clList->first;
	i = 1;	
	while ( user != NULL )
	{
		fd = user->contact->fd;
		sprintf (strInfo, "You are player %d.\n", i);
		write (fd, strInfo, strlen(strInfo) + 1);

		user->number = i++;
		user = user->next;
	}
	free (strInfo);
}
