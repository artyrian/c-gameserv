#include "print.h"
#include "server.h"



/* */
void PrintRandomArray (int * array, int i)
{
	int j;

	j = 0;
	while ( array[j] != 0 )
	{
		printf ("array[%d]=%d.\n", j, array[j]);
		j++;
	}


}



/* */
void PrintListBuy (struct listAuction * auction)
{
	struct auction * buy, * buyR;

	buy = auction->firstBuy;

	while ( buy != NULL )
	{
		buyR = buy;
		while ( buyR != NULL )
		{
			printf ("item=%d, price=%d.",
				buyR->item, buyR->price);
			buyR = buyR->right;
		}
		printf ("\n");
		buy = buy->next;
	}

}



/* */
void PrintListSell (struct listAuction * auction)
{
	struct auction * sell, * sellR;

	sell = auction->firstSell;

	while ( sell != NULL )
	{
		sellR = sell;
		while ( sellR != NULL )
		{
			printf ("item=%d, price=%d.",
				sellR->item, sellR->price);
			sellR = sellR->right;
		}
		printf ("\n");
		sell = sell->next;
	}

	printf ("End of PrintListSell.\n");
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
void PrintClientlist (struct clientlist *clList)
{
	struct client *user;

	user = (struct client *) malloc (sizeof(struct client));
	user = clList->first;

	printf ("Print clientlist:\n");
	while ( user != NULL )
	{
		printf ("FD=%d. #%d. ", 
			user->contact->fd, 
			user->contact->num);

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
		fd = user->contact->fd;
		write (fd, strInfo, strlen(strInfo) + 1);
		user = user->next;
	}
}



/* */
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


/* */
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



/* */
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


/* */
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



/* */
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
