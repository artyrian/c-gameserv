#ifndef _GAME_H_
#define _GAME_H_


#define START_MONEY 10000
#define START_RAW 4
#define START_PRODUCTION 2
#define START_FACTORY 2

#define COSTS_RAW 300
#define COSTS_PRODUCTION 500
#define COSTS_FACTORY 1000

#define PRICE_PROD 2000
#define HALF_PRICE_FACTORY 2500

#define WAIT_BUILD 5


struct fctr
{
	int startMonth;
	struct fctr * next;
};


struct build
{
	struct fctr * first;
	struct fctr * last;
};


struct stuff
{
	int money;
	int raw;
	int order;
	int product;
	int factory;
	int cntBuild;
	struct build * project;
};


struct banker
{
	int state;
	int month;
	struct auction * buy;
	struct auction * sell;
	struct clientlist * clList;
};


void InitStuff (struct stuff *);
void CreateBank (struct banker **, char **);

void GameCycle (struct banker *);
char * GetBuyPrice (struct banker *);
char * GetSellPrice (struct banker *);

#endif
