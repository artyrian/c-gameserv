#ifndef _AUCTION_H_
#define _AUCTION_H_

#include "main.h"
#include "game.h"

//------aucton --------//
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


void Auction (struct banker *);
void InitBuyOrSell (struct auction *);

#endif
