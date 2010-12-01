#include "auction.h"
#include "client.h"
#include "print.h"
#include <time.h>



/* */
void ProvideOneUserBuy (struct auction * buy)
{
	int item;

	item = buy->user->buy->item;
	buy->user->data->raw += item;
	buy->user->data->money -= buy->user->buy->price * item;
	buy->user->buy->item = 0;
}



/* */
void ProvideOneUserSell (struct auction * sell)
{
	int item;

	item = sell->user->sell->item;
	sell->user->data->product -= item;
	sell->user->data->money += sell->user->sell->price * item;
	sell->user->sell->item = 0;
}



/* */
void ProvideUsersBuy (struct listAuction * auction, int sumItem)
{
	struct auction * buy, * tmp;

	buy = auction->firstBuy;
	while ( buy != NULL )
	{
			ProvideOneUserBuy (buy);
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
void ProvideUsersSell (struct listAuction * auction, int sumItem)
{
	struct auction * sell, * tmp;

	sell = auction->firstSell;
	while ( sell != NULL )
	{
			ProvideOneUserSell (sell);
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
void RandomCastingBuy (struct listAuction * auction, int i)
{
	struct auction * buy;
	int j, b, item;	
	int * array;
	
	array = (int *) malloc (i * sizeof(int));

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
			ProvideOneUserBuy (buy);
		}
		else
		{
			break;
		}
	
		j++;
	}
}



/* */
void RandomCastingSell (struct listAuction * auction, int i)
{
	struct auction * sell;
	int j, s, item;	
	int * array;
	
	array = (int *) malloc (i * sizeof(int));

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
			ProvideOneUserSell (sell);
		}
		else
		{
			break;
		}
	
		j++;
	}
}


/* */
void HoldingAuctionBuy (struct listAuction * auction)
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
			ProvideUsersBuy (auction, sumItem);
		}
		else
		{
			RandomCastingBuy (auction, i);
			break;
		}

		buy = buy->next;
	}
}


void HoldingAuctionSell (struct listAuction * auction)
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
			ProvideUsersSell (auction, sumItem);
		}
		else
		{
			RandomCastingSell (auction, i);
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

