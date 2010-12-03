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
			strInfo = (char *) malloc (MESSAGE_LENGHT);
			fd = user->contact->fd;
			sprintf (strInfo, "Made %d production.\n", i);	
			write (fd, strInfo , strlen(strInfo) + 1);
			user->data->order = 0;
			free (strInfo);
		}


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

	HoldingAuctionBuy (auction, bank->clList);
	HoldingAuctionSell (auction, bank->clList);

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
				printf ("check building now\n");
				user->data->money -= HALF_PRICE_FACTORY;
				user->data->factory++;
				user->data->cntBuild--;
			
				DeleteFirstBuildRecord (user);
			}
			construct = construct->next;
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

	if ( completeTurn == bank->clList->cnt 
		&& bank->clList->statusStartGame)
	{
		MonthlyCosts (bank);

		DisableBankrupt (bank);

		Auction (bank);

		TakeOrder (bank);	

		ChangeStateMarket (bank); 

		CheckBuyingFactories (bank);

		DisableBankrupt (bank);
	
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


// ----------------- AUCTION ----------------------------//


/* */
void ProvideOneUserBuy (struct auction * buy, struct clientlist * clList)
{
	int item;
	char * strInfo;

	item = buy->user->buy->item;
	buy->user->data->raw += item;
	buy->user->data->money -= buy->user->buy->price * item;
	buy->user->buy->item = 0;

	strInfo = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strInfo, 
		"Player %d bought: %d raw by %d for one item.\n",
		buy->user->number,
		item,
		buy->user->buy->price
		);
	PrintToAll  (clList, strInfo);
	free (strInfo);
}



/* */
void ProvideOneUserSell (struct auction * sell, struct clientlist * clList)
{
	int item;
	char * strInfo;

	item = sell->user->sell->item;
	sell->user->data->product -= item;
	sell->user->data->money += sell->user->sell->price * item;
	sell->user->sell->item = 0;

	strInfo = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strInfo, 
		"Player %d sold: %d product by %d for one item.\n",
		sell->user->number,
		item,
		sell->user->sell->price
		);
	PrintToAll  (clList, strInfo);
	free (strInfo);

}



/* */
void ProvideUsersBuy (struct listAuction * auction, int sumItem,
			struct clientlist * clList)
{
	struct auction * buy, * tmp;

	buy = auction->firstBuy;
	while ( buy != NULL )
	{
			ProvideOneUserBuy (buy, clList);
			buy = buy->right;
	}

	auction->maxItemSell -= sumItem;
	
	// Delete
	buy = auction->firstBuy;
	auction->firstBuy = auction->firstBuy->next;
	tmp = buy;
	while ( tmp != NULL )
	{
		buy = buy->right;
		free (tmp);
		tmp = buy;
	}
}



/* */
void ProvideUsersSell (struct listAuction * auction, int sumItem,
			struct clientlist * clList)
{
	struct auction * sell, * tmp;

	sell = auction->firstSell;
	while ( sell != NULL )
	{
			ProvideOneUserSell (sell, clList);
			sell = sell->right;
	}

	auction->maxItemBuy -= sumItem;
	
	// Delete
	sell  = auction->firstSell;
	auction->firstSell = auction->firstSell->next;
	tmp = sell;
	while ( tmp != NULL )
	{
		sell = sell->right;
		free (tmp);
		tmp = sell;
	}
}




/* */
void swap (int * array,int j,int r)
{
	int tmp;

	tmp = array[j];
	array[j] = array[r];
	array[r] = tmp;
}




/* */
void CreateRandomArray (int * array, int i)
{
	int j, r;

	srand (time(NULL));

	for ( j = 0; j < i; j++ )
	{
		array[j] = j + 1;
	}
	
	for ( j = 0; j < i; j++ )
	{
		r = rand() % i;
		swap (array, j, r);	
	}

	array[i] = 0;

}



/* */
void RandomCastingBuy (struct listAuction * auction, int i, 
			struct clientlist * clList)
{
	struct auction * buy;
	int j, b, item;	
	int * array;
	
	array = (int *) malloc ((i + 1) * sizeof(int));

	CreateRandomArray (array, i);

	j = 0;
	while ( array[j] != 0 )
	{
		b = 1;
		buy = auction->firstBuy;
		while ( b != array[j] )
		{
			buy = buy->right;
			b++;
		}

		// Provide;
		item = buy->user->buy->item;

		if ( auction->maxItemSell - item >= 0 )
		{
			auction->maxItemSell -= item;
			ProvideOneUserBuy (buy, clList);
		}
		else
		{
			break;
		}
	
		j++;
	}
}



/* */
void RandomCastingSell (struct listAuction * auction, int i, 
			struct clientlist * clList)
{
	struct auction * sell;
	int j, s, item;	
	int * array;
	
	array = (int *) malloc ((i + 1) * sizeof(int));

	CreateRandomArray (array, i);

	j = 0;
	while ( array[j] != 0 )
	{
		s = 1;
		sell = auction->firstSell;
		while ( s != array[j] )
		{
			sell = sell->right;
			s++;
		}

		// Provide;
		item = sell->user->sell->item;

		if ( auction->maxItemBuy - item >= 0 )
		{
			auction->maxItemBuy -= item;
			ProvideOneUserSell (sell, clList);
		}
		else
		{
			break;
		}
	
		j++;
	}
}


/* */
void HoldingAuctionBuy (struct listAuction * auction,
			struct clientlist * clList)
{
	struct  auction * buy, * buyR;
	int sumItem, i;
	
	buy = auction->firstBuy;
	while ( buy != NULL )
	{
		sumItem = 0;
		i = 0;
		buyR = buy;
		while ( buyR != NULL )
		{
			sumItem += buyR->item;	
			i++;
			buyR = buyR->right;
		}
		if ( sumItem <= auction->maxItemSell)
		{
			ProvideUsersBuy (auction, sumItem, clList);
		}
		else
		{
			RandomCastingBuy (auction, i, clList);
			break;
		}

		buy = buy->next;
	}
}


void HoldingAuctionSell (struct listAuction * auction, 
			struct clientlist * clList)
{
	struct  auction * sell, * sellR;
	int sumItem, i;
	
	sell = auction->firstSell;
	while ( sell != NULL )
	{
		sumItem = 0;
		i = 0;
		sellR = sell;
		while ( sellR != NULL )
		{
			sumItem += sellR->item;	
			i++;
			sellR = sellR->right;
		}
		if ( sumItem <= auction->maxItemBuy)
		{
			ProvideUsersSell (auction, sumItem, clList);
		}
		else
		{
			RandomCastingSell (auction, i, clList);
			break;
		}

		sell = sell->next;
	}
}



/* */
int AddToListBuy (struct client * user, struct listAuction * auction)
{
	struct  auction * tmp, * buy, * last;

	tmp = (struct auction *) malloc (sizeof(struct auction));
	tmp->item = user->buy->item; 
	tmp->price = user->buy->price; 
	tmp->user = user;
	tmp->right = NULL;
	tmp->next = NULL;

	if ( auction->firstBuy == NULL)
	{
		auction->firstBuy = tmp;
	}
	else
	{
		if ( tmp->price > auction->firstBuy->price )
		{
			last = auction->firstBuy;
			tmp->next = last;
			auction->firstBuy = tmp;
		}
		else
		{

			buy = auction->firstBuy;
			while ( buy != NULL )
			{
				if ( tmp->price > buy->price )
				{
					tmp->next = buy;
					last->next = tmp;
					return 0;
				}
				if ( tmp->price == buy->price )
				{
					
					while ( buy != NULL )
					{
						last = buy;
						buy = buy->right;
					}
					last->right = tmp;
					return 0;
				}
				last = buy;
				buy = buy->next;
			}
			last->next = tmp;

		}
	}

	return 0;
}



/* */
int AddToListSell (struct client * user, struct listAuction * auction)
{
	struct  auction * tmp, * sell, * last;

	tmp = (struct auction *) malloc (sizeof(struct auction));
	tmp->item = user->sell->item; 
	tmp->price = user->sell->price; 
	tmp->user = user;
	tmp->right = NULL;
	tmp->next = NULL;

	if ( auction->firstSell== NULL)
	{
		auction->firstSell= tmp;
	}
	else
	{
		if ( tmp->price < auction->firstSell->price )
		{
			last = auction->firstSell;
			tmp->next = last;
			auction->firstSell = tmp;
		}
		else
		{

			sell = auction->firstSell;
			while ( sell != NULL )
			{
				if ( tmp->price < sell->price )
				{
					tmp->next = sell;
					last->next = tmp;
					return 0;
				}
				if ( tmp->price == sell->price )
				{
					
					while ( sell != NULL )
					{
						last = sell;
						sell = sell->right;
					}
					last->right = tmp;
					return 0;
				}
				last = sell;
				sell = sell->next;
			}
			last->next = tmp;

		}
	}

	return 0;

}



/* */
void FreeListBuy (struct listAuction * auction)
{
	struct auction * buy, * buyR, * tmp, * tmpR;

	tmp = auction->firstBuy;
	buy = auction->firstBuy;
	while ( tmp != NULL )
	{
		buyR = buy;
		tmpR = buyR;
		while ( tmpR != NULL )
		{
			buyR = buyR->right;
			free (tmpR);
			tmpR = buyR;
		}
		buy = buy->next;
		tmp = buy;
	}

	auction->firstBuy = NULL;
	
}



/* */
void FreeListSell (struct listAuction * auction)
{
	struct auction * sell, * sellR, * tmp, * tmpR;

	tmp = auction->firstSell;
	sell = auction->firstSell;
	while ( tmp != NULL )
	{
		sellR = sell;
		tmpR = sellR;
		while ( tmpR != NULL )
		{
			sellR = sellR->right;
			free (tmpR);
			tmpR = sellR;
		}
		sell = sell->next;
		tmp = sell;
	}

	auction->firstSell = NULL;
}
