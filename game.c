#include "game.h"
#include "client.h"
#include "server.h"
#include "print.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void BuildFactoryToEnd (struct client * user);
int UsersTurnOn (struct banker *);
void TakeOrder (struct banker *);
int MakeProd (struct stuff *);
void MonthlyCosts (struct banker *);
void DisableBankrupt (struct banker *);
void ChangeStateMarket (struct banker *);
void CheckCountOfPlayers (struct banker *);
void TurnOffToAllPlayers (struct banker *);
void InitBank (struct banker *, char **);
void InitStuff (struct stuff *);
void CreateBank (struct banker **, char **);
void FollowBankAuction (struct banker *);
void DeleteFirstBuildRecord (struct client *);
void CheckBuyingFactories (struct banker *);
void GameCycle (struct banker *);
char * GetBuyPrice (struct banker *);
char * GetSellPrice (struct banker *);



/* */
void GameCycle (struct banker * bank)
{
	int completeTurn;
	
	FollowBankAuction (bank);

	completeTurn = UsersTurnOn (bank);

	if ( completeTurn == bank->clList->cnt 
		&& bank->clList->statusStartGame)
	{
		MonthlyCosts (bank);

		Auction (bank);

		TakeOrder (bank);	

		CheckBuyingFactories (bank);

		DisableBankrupt (bank);

		ChangeStateMarket (bank); 
		
		CheckCountOfPlayers (bank);

		TurnOffToAllPlayers (bank);

		PrintToAll (bank->clList, "\n>");
	}
}



/* Count how many user turn on
 */
int UsersTurnOn (struct banker * bank)
{
	int completeTurn;
	struct client * user;

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
	
	return completeTurn;
}



/* */
void TakeOrder (struct banker * bank)
{
	struct client * user;
	struct stuff * data;
	int i;
	int fOrder;

	user = bank->clList->first;
	while ( user != NULL )
	{
		
		data = user->data;
		for ( i = 0; i < data->order; i++)
		{
			if ( data->money >= PRICE_PROD )
			{
				fOrder = MakeProd (data);
			}
			else
			{
				fOrder = 0;
			}
		}
		
		if ( fOrder != 0 )
		{
			PrintMadeProd (user, i); 
		}

		user = user->next;
	}
}



/*
 */
 int MakeProd (struct stuff * data)
 {
	data->money -= PRICE_PROD;
	data->raw --;
	data->product ++;

	return 1;
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
	
	user = bank->clList->first;
	while ( user != NULL )
	{
		if ( user->data->money <= 0 )
		{
			bank->clList->current = user;
			PrintBankrupt (user);

			DisconnectClient (bank->clList);
			user = bank->clList->first;
		}
		else
		{
			user = user->next;
		}
	}
}



/* Inc month, prefet new state of market, Print to all about new month
 */
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
}



/* If left one player - congratulate him.
 */
void CheckCountOfPlayers (struct banker * bank)
{
	if ( bank->clList->cnt == 1 && bank->clList->statusStartGame == 1)
	{
		strWin= (char *) malloc (MESSAGE_LENGHT);
		sprintf (strWin, "\nCongratulate! You are win!\n");
		PrintToAll (bank->clList, strWin);
		free (strWin);
		exit (1);
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
	data->cntBuild = 0;
	data->project = (struct build *) malloc (sizeof(struct build));
	data->project->first = data->project->last = NULL;
}


/* */
void CreateBank (struct banker ** bank, char ** argv)
{
	*bank = (struct banker *) malloc (sizeof(struct banker));
	InitBank (*bank, argv);
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


void DeleteFirstBuildRecord (struct client * user)
{
	struct fctr * tmp;

	tmp = user->data->project->first;
	tmp = tmp->next;
	free (user->data->project->first);
	user->data->project->first = tmp;
}



/* */
void CheckBuyingFactories (struct banker * bank)
{
	struct client * user;
	struct fctr * construct;

	user = bank->clList->first;
	
	while ( user != NULL )
	{
		construct = user->data->project->first;
		while ( construct != NULL )
		{
			if (bank->month-construct->startMonth == WAIT_BUILD)
			{
				BuildFactoryToEnd (user);
				DeleteFirstBuildRecord (user);
			}
			construct = construct->next;
		}
		user = user->next;
	}
}


void BuildFactoryToEnd (struct client * user)
{
	user->data->money -= HALF_PRICE_FACTORY;
	user->data->factory++;
	user->data->cntBuild--;
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
