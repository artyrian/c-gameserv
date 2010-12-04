#include "auction.h"
#include "client.h"
#include "print.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>


void Auction (struct banker *);
int AddToListBuy (struct client *, struct listAuction *);
int AddToListSell (struct client *, struct listAuction *);
void HoldingAuctionBuy (struct listAuction *, struct clientlist *);
void HoldingAuctionSell (struct listAuction *, struct clientlist *);
void FreeListBuy (struct listAuction *);
void FreeListSell (struct listAuction *);
void ProvideOneUserBuy (struct auction *, struct clientlist *);
void ProvideOneUserSell (struct auction *, struct clientlist *);
void CreateRandomArray (int *, int);


/* */
void Auction (struct banker * bank)
{
	struct client * user;
	struct listAuction * auction;

	PrintToAll (bank->clList, "\nAuction info:\n");

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
void InitBuyOrSell(struct auction * buyOrSell)
{
	buyOrSell->item = 0;
	buyOrSell->price = 0;
}




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
		"Player %d bought: raw | price for one\n$\t\t%d\t%d\n",
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
		"Player %d sold: product | price for one\n$\t\t%d\t%d\n",
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
	{ // if list null
		auction->firstBuy = tmp;
	}
	else
	{
		if ( tmp->price > auction->firstBuy->price )
		{ // add first
			last = auction->firstBuy;
			tmp->next = last;
			auction->firstBuy = tmp;
		}
		else
		{ // find where add

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
				{ // go right
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
			// add to last
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
