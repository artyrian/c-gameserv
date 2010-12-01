#ifndef _AUCTION_H_
#define _AUCTION_H_

#include "main.h"
#include "game.h"

struct listAuction
{
	int maxItemBuy;
	int maxItemSell;
	struct auction * firstBuy;
	struct auction * firstSell;
};

struct auction
{
	int item;
	int price;
	struct client * user;
	struct auction * next;
	struct auction * right;
};



int AddToListBuy (struct client *, struct listAuction *);
int AddToListSell (struct client *, struct listAuction *);
void FreeListBuy (struct listAuction *);
void FreeListSell (struct listAuction *);

void HoldingAuctionBuy (struct listAuction *);
void HoldingAuctionSell (struct listAuction *);
#endif

