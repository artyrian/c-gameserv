#include "game.h"
#include "client.h"
#include "server.h"
#include "print.h"
#include <time.h>



/* */
void InitBuyOrSell(struct auction * buyOrSell)
{
	buyOrSell->item = 0;
	buyOrSell->price = 0;
}



/* */
void TakeOrder (struct banker * bank)
{
	struct client * user;
	struct stuff * data;
	char * strInfo;
	int order, i, fd;
	int fOrder;

	user = bank->clList->first;
	while ( user != NULL )
	{
		strInfo= (char *) malloc (MESSAGE_LENGHT);
		
		fOrder = 0;
		data = user->data;
		order = data->order;
		for ( i = 0; i < order; i++)
		{
			if ( data->money >= PRICE_PROD )
			{
				data->money -= PRICE_PROD;
				data->raw --;
				data->product ++;
				fOrder = 1;
			}
		}
		
		if ( fOrder != 0 )
		{
			fd = user->contact->fd;
			sprintf (strInfo, "Made %d production.\n", i);	
			write (fd, strInfo , strlen(strInfo) + 1);
			user->data->order = 0;
		}

		free (strInfo);

		user = user->next;
	}
}



/* */
void MonthlyCosts (struct banker * bank)
{
	struct client * user;
	int costs;

	user = bank->clList->first;
	while ( user != NULL)
	{
		costs = (user->data->raw * COSTS_RAW) 
			+ (user->data->product * COSTS_PRODUCTION) 
			+ (user->data->factory * COSTS_FACTORY);
		user->data->money -= costs;

		user = user->next;
	}
}



/* */
void DisableBankrupt (struct banker * bank)
{
	struct client * user;
	char * strInfo;
	int fd;
	
	user = bank->clList->first;
	while ( user != NULL )
	{
		if ( user->data->money <= 0 )
		{
			strInfo = (char *) malloc (MESSAGE_LENGHT);
			bank->clList->current = user;
			fd = user->contact->fd;
			sprintf (strInfo, "You are bankrupt!\n");
			write (fd, strInfo, strlen(strInfo) + 1);
			DisconnectClient (bank->clList);
			free (strInfo);
			user = bank->clList->first;
		}
		else
		{
			user = user->next;
		}
	}
}



/* */
void ChangeStateMarket (struct banker * bank)
{
	int r, sum, i;
	char * strInfo, * strWin;
	
	const int level_change[5][5] = 
	{
		{ 4, 4, 2, 1, 1 },
		{ 3, 4, 3, 1, 1 },
		{ 1, 3, 4, 3, 1 },
		{ 1, 1, 3, 4, 3 },
		{ 1, 1, 2, 4, 4 }
	};
	
	bank->month ++;

	srand (time(NULL));
	r = 1 + (int)(12.0*rand()/(RAND_MAX+1.0));
	
	sum = 0;
	i = 0;
	while ( r > sum )
	{
		sum += level_change[bank->state][i++];
	}

	bank->state = i;
	printf ("New state of market is %d\n", i);
	
	strInfo = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strInfo, ">= = = %dth month. = = =<\n", bank->month);
	PrintToAll (bank->clList, strInfo);

	free (strInfo);


	if ( bank->clList->cnt == 1 && bank->clList->statusStartGame == 1)
	{
		strWin= (char *) malloc (MESSAGE_LENGHT);
		sprintf (strWin, "\nCongratulate! You are win!\n");
		PrintToAll (bank->clList, strWin);
		free (strWin);
	}
}



/* */
void TurnOffToAllPlayers (struct banker * bank)
{
	struct client * user;

	user = bank->clList->first;

	while ( user != NULL )
	{
		user->f->turn = 0;
		user = user->next;
	}
}


/* */
void InitBank (struct banker * bank, char ** argv)
{
	bank->state = 3;
	bank->month = 1;
	bank->buy = (struct auction *) malloc (sizeof(struct auction));
	bank->sell = (struct auction *) malloc (sizeof(struct auction));
 	CreateClientList ((&bank->clList), argv);
}



/* */
void InitStuff (struct stuff * data)
{
	data->money = START_MONEY;
	data->raw = START_RAW;
	data->order = 0;
	data->product = START_PRODUCTION;
	data->factory = START_FACTORY;
}


/* */
void CreateBank (struct banker ** bank, char ** argv)
{
	*bank = (struct banker *) malloc (sizeof(struct banker));
	InitBank (*bank, argv);
}



/* */
void Auction (struct banker * bank)
{
	struct client * user;
	struct listAuction * auction;

	auction = (struct listAuction *) malloc(sizeof(struct listAuction));
	auction->firstBuy = auction->firstSell = NULL;
	auction->maxItemBuy = bank->buy->item;
	auction->maxItemSell = bank->sell->item;

	user = bank->clList->first;
	while ( user != NULL )
	{
		if ( user->buy->item != 0 )
		{
			AddToListBuy (user, auction);
		}

		if ( user->sell->item != 0 )
		{
			AddToListSell (user, auction);
		}

		user = user->next;
	}

	HoldingAuctionBuy (auction);
	HoldingAuctionSell (auction);

	FreeListBuy (auction);
	FreeListSell (auction);
}



/* */
void FollowBankAuction (struct banker * bank)
{
	double P;
	const double value[5][4] = 
	{
		{ 1.0,	800, 	3.0, 	6500 },
		{ 1.5, 	650, 	2.5, 	6000 },
		{ 2.0, 	500,	2.0, 	5500 },
		{ 2.5,	400,	1.5,	5000 },
		{ 3.0,	300,	1.0,	4500 }
	};

	if ( bank->clList->statusStartGame != 0 )
	{
		P = bank->clList->cnt;
		bank->sell->item = P * value[bank->state-1][0];
		bank->sell->price = value[bank->state-1][1]; 
		bank->buy->item = P * value[bank->state-1][2];
		bank->buy->price = value[bank->state-1][3]; 
	}
}


void DeleteFirstBuildRecord (struct banker * bank)
{
	struct build * tmp;
	struct client * user;

	user = bank->clList->first;
	tmp = user->data->project;
	tmp = tmp->next;
	free (user->data->project);
	user->data->project = tmp;
}



/* */
void CheckBuyingFactories (struct banker * bank)
{
	struct client * user;
	struct build * project;

	user = bank->clList->first;
	
	while ( user != NULL )
	{
		project = user->data->project;
		while ( project != NULL )
		{
			if ( bank->month - project->startMonth == 5 )
			{
				user->data->money -= HALF_PRICE_FACTORY;
				user->data->factory++;
				user->data->cntBuild--;
			
				DeleteFirstBuildRecord (bank);
			}
			project = project->next;
		}
		user = user->next;
	}
}


/* */
void GameCycle (struct banker * bank)
{
	struct client * user;
	int completeTurn;
	
	FollowBankAuction (bank);

	completeTurn = 0;
	user = bank->clList->first;
	while ( user != NULL )
	{
		if ( user->f->turn != 0 )
		{
			completeTurn++;
		}
		user = user->next;
	}

	if ( completeTurn == bank->clList->cnt )
	{
		Auction (bank);

		TakeOrder (bank);	

		MonthlyCosts (bank);

		DisableBankrupt (bank);

		ChangeStateMarket (bank);

		CheckBuyingFactories (bank);
	
		TurnOffToAllPlayers (bank);

		PrintToAll (bank->clList, "\n>");
	}
}



/* */
char * GetBuyPrice (struct banker * bank)
{
	char * strInfo;
	int item, price, P;

	P = bank->clList->cnt;
	item = bank->buy->item; 
	price = bank->buy->price; 

	strInfo = (char *) malloc (MESSAGE_LENGHT * 2);
	sprintf (strInfo, "Bank buys:\tItems\tMax.price\n$\t\t%d\t%d\n",
		item, price	
		);

	return strInfo;
}



/* */
char * GetSellPrice (struct banker * bank)
{
	char * strInfo;
	int item, price, P;

	P = bank->clList->cnt;
	item = bank->sell->item; 
	price = bank->sell->price; 

	strInfo = (char *) malloc (MESSAGE_LENGHT * 2);
	sprintf (strInfo, "Bank sells:\tItems\tMin.price\n$\t\t%d\t%d\n",
		item, price
		);

	return strInfo;
}
