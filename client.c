#include "client.h"
#include "server.h"
#include "print.h"
#include "commands.h"
#include "auction.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


void CreateClientList (struct clientlist **, char **);
int MaxDescriptor (struct clientlist *, fd_set *, int);
void AcceptQuery (int, struct clientlist *);
void ConnectClient (struct clientlist *, int);
void DisconnectClient (struct clientlist *);
void DeniedClient (int);
int ReadToBuffer (struct client *, int);
char * StatusUsersConnecting (struct clientlist *);
char * StatusUsersPlaying (struct clientlist *);
char * GetInfoPlayer (struct client *);
void AddClientToList (struct client *, struct clientlist *);
void InitFlags (struct userFlags *);
void InitUser (struct client *, struct clientlist *, int);
void ReadyToStartGame (struct clientlist *);
void GiveUserId (struct clientlist *);
void DeleteCurrentClientFromList (struct clientlist *);
void CheckCountOfUsers (struct clientlist *);



/* Malloc memory for list of clients, NULL ptrs for 1th and last.
 * set maxPlayers, and status of start game (0 for now) 
 */
void CreateClientList (struct clientlist ** clList, char ** argv)
{
	(*clList) = (struct clientlist *)malloc(sizeof(struct clientlist));
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
		{
			max_d = fd;
		}
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




/* Add client to struct, Init, print about it, check for start game.
 */
void ConnectClient (struct clientlist *clList, int fd)
{
	struct client * user;

	user = (struct client *) malloc (sizeof(struct client));

	AddClientToList (user, clList);

	InitUser (user, clList, fd);

	printf ("New client connected. FD=%d. #%d.\n", fd, clList->cnt);

	PrintToFDSuccessConnect (fd);
	PrintToAll(clList, StatusUsersConnecting(clList));
	
	if ( clList->cnt == clList->maxPlayers )
	{
		ReadyToStartGame (clList);
	}
}	




/* Delete current user
 * close fd, print current cnt to all,
 * check cnt of all of zero.
 */
void DisconnectClient (struct clientlist *clList)
{
	int fd;
	
	DeleteCurrentClientFromList (clList);	

	fd = clList->current->contact->fd; 
	shutdown(fd, 2);
	close (fd);
	printf ("Client disconnected. FD=%d.\n", fd);

	//Must be fn FreeUser (struct clList *);

	free(clList->current->buf->str);
	free(clList->current->buf);
	free(clList->current);

	clList->cnt --;
	printf ("Clients now:%d.\n", clList->cnt);

	CheckCountOfUsers (clList);
}



/* Fn write to fd that access denied
 */
void DeniedClient (int fd)
{
	const char strFullServer[60] = "Sorry! The server is full or game started.\n";
	write (	fd, strFullServer, strlen(strFullServer) + 1);
}



/* Return:
 * -1 if error read (perror)
 * 0 close fd (check not here)
 * >0 read from fd one symbol. write to buffer then check overflow buf
 * and extend if it need.
 */
int ReadToBuffer (struct client * user, int fd)
{
	int rc;
	char c; 
	
	user->buf->str = user->buf->str;

	rc = read (fd, &c, sizeof(char));
	if ( rc == -1 )
	{
		perror ("read");
	}
	
	if ( user->buf->cnt  == BUF_SIZE * user->buf->part - 1)
	{
		ExtendBuffer (user->buf);
	}

	if ( rc != 0 )
	{
		user->buf->str[user->buf->cnt++] = c;
		user->buf->str[user->buf->cnt] = '\0';
	}

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



/* Return string with his gamestatic ("player X") of current user */
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



/*
 * Begin of fn ConnectClient ()
 */



/* Add user to list,
 * change ptr first or last 
 * incremebt count of users
 */
void AddClientToList (struct client * user, struct clientlist * clList)
{

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
}



/* Initializating all flags 
 */
void InitFlags (struct userFlags * f)
{
	f->turn = 0;
}



/* Init most of all firms of client
 */
void InitUser (struct client * user, struct clientlist *clList, int fd)
{
	//user->number = clList->cnt; It's not need.fn better GetUserId()
	user->next = NULL;

	user->contact = (struct settings*)malloc(sizeof(struct settings));
	InitSettings (user->contact, fd, clList->cnt);

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



/* Call fn GiveUserId(), 
 * change statusStartGame,
 * print all about start.
 */
void ReadyToStartGame (struct clientlist * clList) 
{
	GiveUserId (clList);
	clList->statusStartGame = 1;
	PrintToAll ( clList, 
		     "The game start now.\n>= = = 1th month. = = =<\n>"
		   );
}



/* Put to all users ID from 1 to N and write them*/
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



/*
 * End of fn ConnectClient ()
 */	



/*
 * BEGIN OF FUNCTIONS FOR DisconnectClient (sturct clientlist *)
 */


/* Delete from struct current user. if first - spec.proccessing
 * else simple delete and change pointer to last
 */
void DeleteCurrentClientFromList (struct clientlist * clList)
{
	struct client * cur, * prev, * tmp;

	if ( clList->current == clList->first)
	{
		tmp = clList->current->next;;
		clList->first = tmp;
	}
	else
	{
		cur = clList->first;
		while ( cur != clList->current )
		{
			prev = cur;
			cur = cur->next;
		}
		tmp = cur->next;
		prev->next = tmp;

		if ( cur == clList->last )
		{
			clList->last = prev;	
		}
	}
}



/* Check cnt and say to all current cnt 
 * or print in server "all went out"
 */
void CheckCountOfUsers (struct clientlist * clList)
{
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

/*
 * END OF FUNCTIONS FOR DisconnectClient (sturct clientlist *)
 */


