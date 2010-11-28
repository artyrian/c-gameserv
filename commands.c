#include "commands.h"
#include "server.h"



/* */
void InitCommand (struct command * cmd)
{
	cmd->cnt = 0;
	cmd->last = cmd->first = NULL;
}

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
void IncrementVar ()
{
	global_i++;
}



/* */
void PrintVar (int fd)
{
	char * str;
	str = (char *) malloc (MESSAGE_LENGHT);

	sprintf (str, "%d\n", global_i);

	write (fd, str, strlen(str) + 1); 

	free (str);
}


/* */
void GameNotStarted (int fd)
{
	char * str;
	str = (char *) malloc (MESSAGE_LENGHT);

	sprintf (str, "Game not started.\nYou can't use any commands.\n");

	write (fd, str, strlen(str) + 1); 

	free (str);

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
void SendPlayerInfo (struct banker * bank, int numPlayer)
{
	struct client * user;
	char * strInfo, * strWrong;
	int fd;

	strWrong = (char *) malloc (MESSAGE_LENGHT);
	sprintf (strWrong, "Player with number %d not found.\n", numPlayer);

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
	
	data = bank->clList->current->data;
	if ( data->order < data->factory ) 
	{
		data->order ++;
	}
}


/* */
void Buy (struct banker * bank)
{
	struct client * user;
	struct command * cmd;
	int number, price;

	cmd = bank->clList->current->cmd;
	user = bank->clList->current;
	if ( cmd->first->next != NULL )
	{
		cmd->first = cmd->first->next;	

		number = atoi (cmd->first->str);
		if ( number > 0 && cmd->first->next != NULL )
		{
			user->buy->number = number;
			cmd->first = cmd->first->next;	

			price = atoi (cmd->first->str);
			if (price > 0 )
				user->buy->price = price;
			else 
				PrintHelp (bank);
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
void Sell (struct banker * bank)
{
	struct client * user;
	struct command * cmd;
	int number, price;

	cmd = bank->clList->current->cmd;
	user = bank->clList->current;
	if ( cmd->first->next != NULL )
	{
		cmd->first = cmd->first->next;	

		number = atoi (cmd->first->str);
		if ( number > 0 && cmd->first->next != NULL )
		{
			user->sell->number = number;
			cmd->first = cmd->first->next;	

			price = atoi (cmd->first->str);
			if (price > 0 )
				user->sell->price = price;
			else 
				PrintHelp (bank);
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

void Build (struct banker * bank)
{

}



/* */
void TurnStep (struct banker * bank)
{
	bank->clList->current->f->turn = 1;
}



/* */
void Help (int fd)
{
	const char strHelp[NUMBER_OF_COMMANDS * MESSAGE_LENGHT] = 
	"It's HELP to commands. Write:\nINC\t- to increment global i.\nPRINT\t- print global i.\nMARKET\t- show information about market.-+\nPLAYER N- show information about bussines of player #N(positive int).\nPROD\t- make 1 product (need 1 factory, 1 raw, 2000$).\nBUY N M\t- buy N row by M dollars.-\nSELL N M- sell N production by M dollars.-\nBUILD\t- build.-\nTURN\t- end of this round.\nHELP\t- to call this help.\n\0"
	;
	
	write (fd, &strHelp, strlen(strHelp) + 1);
}
/* */
void PrintHelp (struct banker * bank)
{
	int fd;
	const char strHelp[MESSAGE_LENGHT] =
	"Call help you can with command HELP\n\0";

	fd = bank->clList->current->contact->fd;
	write (fd, &strHelp, strlen(strHelp) + 1);
}
