#include "print.h"
#include "server.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void PrintWrongPriceBuy (struct banker *);
void PrintWrongItemBuy (struct banker *);
void PrintWrongPriceSell (struct banker *);
void PrintWrongItemSell (struct banker *);
void PrintCantCreateProd (struct banker *);
void PrintWillCreateProd (struct banker *);
void PrintBankrupt (struct client *);
void PrintMadeProd (struct client *, int i);
void PrintGameNotStarted (int);
void PrintServerInfo (int, int);
void PrintDescriptors (int *, int);
void PrintToFDSuccessConnect (int);
void PrintToAll (struct clientlist *, char *);
void PrintSuccessBuy (struct clientlist *);
void PrintSuccessSell (struct clientlist *);
void PrintSuccessBuild (struct clientlist *);
void PrintTurnOn (struct clientlist *);
void PrintComing (int, int);



/*
 */
void PrintWrongPriceBuy (struct banker * bank)
{
	int fd;
	char * strInfoPrice;
		fd = bank->clList->current->contact->fd;
		strInfoPrice = (char *) malloc (MESSAGE_LENGHT);
		sprintf (strInfoPrice, 
			"Wrong price! Max. price:\t%d.\n", 
			bank->buy->price
			);
		write (fd, strInfoPrice, strlen (strInfoPrice)+1);
		free (strInfoPrice);
}



/*
 */
void PrintWrongItemBuy (struct banker * bank)
{
	int fd;
	char * strInfoItem;
	fd = bank->clList->current->contact->fd;
	strInfoItem = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strInfoItem, 
		"Wrong item! Max. item:\t%d.\n", 
		bank->buy->item
		);
	write (fd, strInfoItem, strlen (strInfoItem) + 1);
	free (strInfoItem);

}


/*
 */
void PrintWrongPriceSell (struct banker * bank)
{
	int fd;
	char * strInfoPrice;

	fd = bank->clList->current->contact->fd;
	strInfoPrice = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strInfoPrice, 
		"Wrong price! Min. price:\t%d.\n", 
		bank->sell->price
		);
	write (fd, strInfoPrice, strlen (strInfoPrice)+1);
	free (strInfoPrice);
}



/* 
 */
void PrintWrongItemSell (struct banker * bank)
{
	int fd;
	char * strInfoItem;

	fd = bank->clList->current->contact->fd;
	strInfoItem = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strInfoItem, 
		"Wrong item! Max. item:\t%d.\n", 
		bank->sell->item	
		);
	write (fd, strInfoItem, strlen (strInfoItem) + 1);
	free (strInfoItem);
}




/* Print to current user that he can't create prod,
 * because factory not enough.
 */
void PrintCantCreateProd (struct banker * bank)
{
	char * strInfo;
	int fd;

	fd = bank->clList->current->contact->fd;
	strInfo = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strInfo, 
		"You can't create product in month more than have factory\n"
		);

	write (fd, strInfo, strlen(strInfo) + 1);
	free (strInfo);
}



/* Print prod which may be will be create
*/
void PrintWillCreateProd (struct banker * bank)
{
	struct stuff * data;
	char * strInfo;
	int fd;

	data = bank->clList->current->data;
	fd = bank->clList->current->contact->fd;
	strInfo = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strInfo, "You'll create %d production in all.\n",
		data->order);	
	write (fd, strInfo, strlen(strInfo) + 1);
	free (strInfo);
}



/* Print to user that he is bankrupt
 */
void PrintBankrupt (struct client * user)
{
	char * strInfo;
	int fd;

	strInfo = (char *) malloc (MESSAGE_LENGHT);
	fd = user->contact->fd;
	sprintf (strInfo, "You are bankrupt!\n");
	write (fd, strInfo, strlen(strInfo) + 1);
	free (strInfo);
}



/* 
 */
void PrintMadeProd (struct client* user, int i)
{
	int fd;
	char * strInfo;

	strInfo = (char *) malloc (MESSAGE_LENGHT);
	fd = user->contact->fd;
	sprintf (strInfo, "Made %d production.\n", i);
	write (fd, strInfo , strlen(strInfo) + 1);
	user->data->order = 0;
	free (strInfo);
}
	


/* Print to fd that game not started.
 */
void PrintGameNotStarted (int fd)
{
	char * str;
	str = (char *) malloc (MESSAGE_LENGHT);

	sprintf (str, "Game not started.\nYou can use only \"help\".\n");

	write (fd, str, strlen(str) + 1); 

	free (str);

}



/* Print in server maxPlayers and ports got from argv.
 */
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



/* Print to fd that client is connected.
 */
void PrintToFDSuccessConnect (int fd)
{
	char * strConnected;
	strConnected = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strConnected, "You're connected to server.\n");
	write ( fd, strConnected, strlen(strConnected) + 1 );
	free (strConnected);
}



/* Print to all char * strInfo;
 */
void PrintToAll(struct clientlist * clList, char * strInfo)
{
	struct client * user;
	int fd;

	user = clList->first;

	while ( user != NULL )
	{
		fd = user->contact->fd;
		write (fd, strInfo, strlen(strInfo) + 1);
		user = user->next;
	}
}



/* Print to cur client that lots buy get.
 */
void PrintSuccessBuy (struct clientlist *clList)
{
	struct client * user;
	char * strInfo;
	int fd;

	user = clList->current;
	fd = user->contact->fd;
	strInfo = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strInfo, 
		"Lots to buy get. All previous buy-lots lost.\n"
		);
	write (fd, strInfo, strlen(strInfo) + 1);
	free (strInfo);
}


/* Print to cur client that lots sell get
 */
void PrintSuccessSell (struct clientlist * clList)
{
	struct client * user;
	char * strInfo;
	int fd;

	user = clList->current;
	fd = user->contact->fd;
	strInfo = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strInfo, 
		"Lots to sell get. All previous sell-lots lost.\n"
		);
	write (fd, strInfo, strlen(strInfo) + 1);
	free (strInfo);
}



/* Print to cur client about start building factory;
 */
void PrintSuccessBuild (struct clientlist * clList)
{
	struct client * user;
	char * strInfo;
	int fd;

	user = clList->current;
	fd = user->contact->fd;
	strInfo = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strInfo, 
		"You start build factory. After 5 months consctruc end.\n"
		);
	write (fd, strInfo, strlen(strInfo) + 1);
	free (strInfo);

}


/* Print to current user that "turn" used. 
 */
void PrintTurnOn (struct clientlist * clList)
{
	struct client * user;
	char * strInfo;
	int fd;

	user = clList->current;
	fd = user->contact->fd;
	strInfo = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strInfo, 
		"Turn cmd get. Now you can use only info-cmd before change month.\n"
		);
	write (fd, strInfo, strlen(strInfo) + 1);
	free (strInfo);

}



/* In fd print coming (if game not started).
*/
void PrintComing (int fd, int statusStartGame)
{
	char * strIn;

	strIn = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strIn, "\n>");
	
	if ( statusStartGame != 0 )
	{
		write (fd, strIn, strlen(strIn) + 1);
	}
	free (strIn);
}
