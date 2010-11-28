#include "game.h"
#include "client.h"
#include "server.h"
#include <time.h>

/* */
void TakeOrder (struct banker * bank)
{
	struct client * user;
	struct stuff * data;
	char * strInfo;
	int order;
	int i;
	int fd;

	user = bank->clList->first;
	while ( user != NULL )
	{
		strInfo= (char *) malloc (MESSAGE_LENGHT);
		
		data = user->data;
		order = data->order;
		for ( i = 0; i < order; i++)
		{
			if ( data->money >= PRICE_PROD )
			{
				data->money = data->money - PRICE_PROD;
				data->raw --;
				data->product ++;

			}
		}
		
		if ( order != 0 )
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
		user->data->money = user->data->money - costs;

		user = user->next;
	}
}


/* */
void ChangeStateMarket (struct banker * bank)
{
	int r, sum, i;
	
	const int level_change[5][5] = 
	{
		{ 4, 4, 2, 1, 1 },
		{ 3, 4, 3, 1, 1 },
		{ 1, 3, 4, 3, 1 },
		{ 1, 1, 3, 4, 3 },
		{ 1, 1, 2, 4, 4 }
	};

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

}



/* */
void InitBank (struct banker * bank, char ** argv)
{
	bank->state = 3;
	bank->month = 1;
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

void GameCycle (struct banker * bank)
{
	struct client * user;
	int completeTurn;

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
		user = bank->clList->first;
		while ( user != NULL )
		{
			MonthlyCosts (bank);
			user->f->turn = 0;
			user = user->next;
		}

		bank->month ++;
		ChangeStateMarket (bank);

		TakeOrder (bank);	
	}
}



/* */
char * GetBuyPrice (struct banker * bank)
{
	char * strInfo;
	int item, price, P;
	const double value[5][4] = 
	{
		{ 1.0,	800, 	3.0, 	6500 },
		{ 1.5, 	650, 	2.5, 	6000 },
		{ 2.0,	500,	2.0,	5500 },
		{ 2.5,	400,	1.5,	5000 },
		{ 3.0,	300,	1.0,	4500 }
	};

	P = bank->clList->cnt;
	item = value[bank->state-1][0] * P;
	price = value[bank->state-1][1]; 

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
	const double value[5][4] = 
	{
		{ 1.0,	800, 	3.0, 	6500 },
		{ 1.5, 	650, 	2.5, 	6000 },
		{ 2.0,	500,	2.0,	5500 },
		{ 2.5,	400,	1.5,	5000 },
		{ 3.0,	300,	1.0,	4500 }
	};


	P = bank->clList->cnt;
	item = P * value[bank->state-1][2];
	price = value[bank->state-1][3]; 

	strInfo = (char *) malloc (MESSAGE_LENGHT * 2);
	sprintf (strInfo, "Bank sells:\tItems\tMin.price\n$\t\t%d\t%d\n",
		item, price
		);

	return strInfo;
}
