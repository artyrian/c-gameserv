#include "commands.h"
#include "server.h"
#include "print.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



void InitBuyLot (struct banker *);
void InitSellLot (struct banker *);
void SendPlayerInfo (struct banker *, int );



/* Init cnt of cmd and NULL ptrs for 1st and last cmd
 */
void InitCommand (struct command * cmd)
{
	cmd->cnt = 0;
	cmd->last = cmd->first = NULL;
}



/* Free all cmd (accurately).
 */
void ClearCommand (struct command * cmd)
{
	struct word * tmp;
	tmp = cmd->first;
	while (tmp != NULL)
	{
		tmp = cmd->first->next;
		free (cmd->first->str);
		free (cmd->first);
		cmd->first = tmp;
	}
}





/* */
void Market (struct banker * bank)
{
	char * strPlayers, * strMonth, * strBuy, * strSell,  * str;
	int fd;
	strMonth = (char *) malloc (MESSAGE_LENGHT);

	strPlayers = StatusUsersPlaying(bank->clList);

	sprintf (strMonth, "Current month is %dth.\n", bank->month);

	strBuy = GetBuyPrice (bank);
	strSell = GetSellPrice (bank);

	str = (char *) malloc ( MESSAGE_LENGHT 
				+ strlen(strPlayers) 
				+ strlen(strBuy)
				+ strlen(strSell)
				);

	sprintf (str, "%s%s%s%s", strMonth, strPlayers, strBuy, strSell);

	fd = bank->clList->current->contact->fd;
	write (fd, str, strlen(str) + 1);

	free (strPlayers);
	free (strMonth);
	free (str);
}



/* */
void PlayerInfo (struct banker * bank)
{
	struct command * cmd;
	struct client * user;
	int numPlayer;

	cmd = bank->clList->current->cmd;
	user = bank->clList->first;
	if ( cmd->first->next != NULL )
	{
		cmd->first = cmd->first->next;	
		numPlayer = atoi (cmd->first->str);
		if ( numPlayer > 0 )
		{
			SendPlayerInfo (bank, numPlayer);
		}
		else
		{
			PrintHelp (bank);	
		}
	}
	else
	{
		PrintHelp (bank);	
	}
}



/* */
void Production (struct banker * bank)
{
	struct stuff * data;
	int fd;
	char * strInfo;
	

	data = bank->clList->current->data;
	if ( data->order < data->factory ) 
	{
		data->order ++;
		fd = bank->clList->current->contact->fd;
		strInfo = (char *) malloc (MESSAGE_LENGHT);
		sprintf (strInfo, "You'll create %d production in all.\n",
			data->order);	

		write (fd, strInfo, strlen(strInfo) + 1);
		free (strInfo);

	}
	else 
	{
		fd = bank->clList->current->contact->fd;
		strInfo = (char *) malloc (MESSAGE_LENGHT);
		sprintf (strInfo, 
			"You can't create product in month more than have factory\n");

		write (fd, strInfo, strlen(strInfo) + 1);
		free (strInfo);
	}
}



/* */
void Buy (struct banker * bank)
{
	struct client * user;
	struct command * cmd;

	cmd = bank->clList->current->cmd;
	user = bank->clList->current;
	if ( cmd->first->next != NULL && cmd->first->next->next != NULL )
	{
		InitBuyLot (bank);
	}
	else
	{
		PrintHelp (bank);
	}
}



/* */
void Sell (struct banker * bank)
{
	struct client * user;
	struct command * cmd;

	cmd = bank->clList->current->cmd;
	user = bank->clList->current;
	if ( cmd->first->next != NULL && cmd->first->next->next != NULL )
	{
		InitSellLot (bank);
	}
	else
	{
		PrintHelp (bank);
	}
}



/* */
void Build (struct banker * bank)
{
	struct client * user;
	struct fctr * tmp;

	tmp = (struct fctr *) malloc (sizeof(struct fctr));
	tmp->startMonth = bank->month;
	tmp->next = NULL;

	user = bank->clList->current;
	
	if ( user->data->project->last == NULL )
	{
		user->data->project->first = tmp;
	}
	else
	{
		user->data->project->last->next = tmp;
	}
	user->data->project->last = tmp;

	user->data->money -= HALF_PRICE_FACTORY;
	user->data->cntBuild++;

	PrintSuccessBuild (bank->clList);
}



/* */
void TurnStep (struct banker * bank)
{
	bank->clList->current->f->turn = 1;
	PrintTurnOn (bank->clList);
}



/* */
void WhoAmI (struct banker * bank)
{
	int fd;
	int numberPlayer;
	char * strInfo;


	fd = bank->clList->current->contact->fd;
	numberPlayer = bank->clList->current->number;
	strInfo = (char *) malloc (MESSAGE_LENGHT);

	sprintf (strInfo, "You are PLAYER %d.\n", numberPlayer);
	
	write (fd, strInfo, strlen(strInfo) + 1);

	free (strInfo);
}



/* */
void Help (int fd)
{
	const char strHelp[(NUMBER_OF_COMMANDS+1) * MESSAGE_LENGHT] = 
	"It's help to commands.(i - info cmd, c - control cmd)\nmarket\t- show information about market.(i)\nplayer N- show information about bussines of player #N (positive int).(i)\nprod\t- make 1 product (need 1 factory, 1 raw, 2000$).(c)\nbuy N M\t- buy N row by M dollars.(c)\nsell M- sell N production by M dollars.(c)\nbuild\t- build one more factory (2500$ now, 2500$ after 5 months).(c)\nturn\t- end of this round.(c)\nwhoami\t- watch your current player number.(i)\nhelp\t- call this help.(i)\n\0"
	;
	
	write (fd, &strHelp, strlen(strHelp) + 1);
}



/* */
void PrintHelp (struct banker * bank)
{
	int fd;
	const char strHelp[MESSAGE_LENGHT] =
	"Call help you can with command \"help\" (without quotes)\n\0";

	fd = bank->clList->current->contact->fd;
	write (fd, &strHelp, strlen(strHelp) + 1);
}



/*
 * Functions who helps  to general Fn-cmds
*/



/* */
void InitBuyLot (struct banker * bank)
{
	struct client * user;
	struct command * cmd;
	int item, price, fd;
	char * strInfoPrice, * strInfoItem;


	user = bank->clList->current;
	cmd = bank->clList->current->cmd;

	cmd->first = cmd->first->next;
	item = atoi (cmd->first->str);

	cmd->first = cmd->first->next;
	price = atoi (cmd->first->str);

	if ( (item > 0) && (item <= bank->sell->item) )
	{
		if ( price >= bank->sell->price )
		{
			user->buy->item = item;
			user->buy->price = price;
			PrintSuccessBuy (bank->clList);
		}
		else
		{
			fd = bank->clList->current->contact->fd;
			strInfoPrice = (char *) malloc (MESSAGE_LENGHT);
			sprintf (strInfoPrice, 
				"Wrong price! Min. price:\t%d.\n", 
				bank->sell->price
				);
			write (fd, strInfoPrice, strlen (strInfoPrice)+1);
			free (strInfoPrice);
		}
	}
	else
	{
		fd = bank->clList->current->contact->fd;
		strInfoItem = (char *) malloc (MESSAGE_LENGHT);
		sprintf (strInfoItem, 
			"Wrong item! Max. item:\t%d.\n", 
			bank->sell->item	
			);
		write (fd, strInfoItem, strlen (strInfoItem) + 1);
		free (strInfoItem);
	}

}



/* */
void InitSellLot (struct banker * bank)
{
	struct client * user;
	struct command * cmd;
	int item, price, fd;
	char * strInfoPrice, * strInfoItem;


	user = bank->clList->current;
	cmd = bank->clList->current->cmd;

	cmd->first = cmd->first->next;
	item = atoi (cmd->first->str);

	cmd->first = cmd->first->next;
	price = atoi (cmd->first->str);

	if ( (item > 0) && (item <= bank->buy->item) )
	{
		if ( price <= bank->buy->price )
		{
			user->sell->item = item;
			user->sell->price = price;
			PrintSuccessSell (bank->clList);
		}
		else
		{
			fd = bank->clList->current->contact->fd;
			strInfoPrice = (char *) malloc (MESSAGE_LENGHT);
			sprintf (strInfoPrice, 
				"Wrong price! Max. price:\t%d.\n", 
				bank->buy->price
				);
			write (fd, strInfoPrice, strlen (strInfoPrice)+1);
			free (strInfoPrice);
		}
	}
	else
	{
		fd = bank->clList->current->contact->fd;
		strInfoItem = (char *) malloc (MESSAGE_LENGHT);
		sprintf (strInfoItem, 
			"Wrong item! Max. item:\t%d.\n", 
			bank->buy->item
			);
		write (fd, strInfoItem, strlen (strInfoItem) + 1);
		free (strInfoItem);
	}

}



/* */
void SendPlayerInfo (struct banker * bank, int numPlayer)
{
	struct client * user;
	char * strInfo, * strWrong;
	int fd;

	strWrong = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strWrong, "Player with number %d not found.\n",numPlayer);

	fd = bank->clList->current->contact->fd;
	user = bank->clList->first;
	while (user != NULL)
	{
		if ( user->number == numPlayer )
		{
			strInfo = GetInfoPlayer (user);
			write (fd, strInfo, strlen(strInfo) + 1);	
			free (strInfo);
			break;
		}

		user = user->next;
		if ( user == NULL )
		{
			write (fd, strWrong, strlen(strWrong) + 1);
		}
	}

	free (strWrong);

}




/*
 * Functions who helps  to general Fn-cmds
 */
