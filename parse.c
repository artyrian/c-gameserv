#include "parse.h"
#include "server.h"



/* */
void FindCommandBeforeStart (struct client * user)
{
	char * str;
	int fd;

	fd = user->contact->fd;
	str = user->cmd->first->str;

	printf ("str=%s.", str);
	if ( user->cmd->first != NULL )
	{
		GameNotStarted (fd);
	}	
}



/* */
void FindInfoCommand(struct banker * bank)
{
	struct client * user;
	char * str;
	int i;
	int fd;
	
	user = bank->clList->current;
	fd = user->contact->fd;
	str = user->cmd->first->str;
	i = strlen (user->cmd->first->str) - 1;

	if ( !strncmp (str, "MARKET", i) && i )
	{
		Market (bank);	
	}
	else if ( !strncmp (str, "PLAYER", i) && i )
	{
		PlayerInfo (bank);	
	}
	else if ( !strncmp (str, "WHOAMI", i) && i )
	{
		WhoAmI (bank);
	}
	else if ( !strncmp (str, "INC", i) && i )
	{
		printf ("Now inc\n");	
		IncrementVar ();
	}	
	else if ( !strncmp(str, "PRINT", i) && i )
	{
		printf ("Now print(FD=%d)\n", fd);	
		PrintVar (fd);
	}
	else if ( !strncmp(str, "HELP", i) && i)
	{
		Help (fd);
	}
	else 
		PrintHelp (bank);
	
}



/* */
int FindActionCommand (struct banker * bank)
{
	struct client * user;
	char * str;
	int i;
	int fd;
	int result;
	
	result = 1;
	user = bank->clList->current;
	fd = user->contact->fd;
	str = user->cmd->first->str;
	i = strlen (user->cmd->first->str) - 1;

	if ( !strncmp (str, "PROD", i) && i )
	{
		Production (bank);		
	}
	else if ( !strncmp (str, "BUY", i) && i )
	{
		Buy (bank);	
	}
	else if ( !strncmp (str, "SELL", i) && i )
	{
		Sell (bank);	
	}
	else if ( !strncmp (str, "BUILD", i) && i )
	{
		Build (bank);	
	}
	else if ( !strncmp (str, "TURN", i) && i )
	{
		TurnStep (bank);
	}
	else 
		result = 0;	
	
	return result;
}




/* */
void FindCommand (struct banker * bank)
{
	struct command * cmd;

	cmd = bank->clList->current->cmd;
	if ( cmd->cnt != 0 ) 
	{
		if (bank->clList->statusStartGame == 0)
		{
			FindCommandBeforeStart (bank->clList->current);
		}
		else if (bank->clList->current->f->turn != 0)
		{
			FindInfoCommand(bank);	
		}
		else
		{
			if ( FindActionCommand (bank) == 0 )
				FindInfoCommand (bank);
		}
	}
}



/* */
void AddWordToStructure (struct clientlist * clList, char * str, int i)
{
	struct word * word;
	word = (struct word * ) malloc (sizeof(struct word));
	word->str = (char *) malloc (i * sizeof(char));
	word->next = NULL;

	strcpy (word->str, str);

	clList->current->cmd->cnt++;

	if ( clList->current->cmd->first == NULL )
		clList->current->cmd->first = word;
	else
		clList->current->cmd->last->next = word;	
	
	clList->current->cmd->last = word;	
}


/* */
char SkipSpaces (struct clientlist * clList, char c, int * i)
{
	while ( c == '\t' || c == ' ' || c == '\n' ) 
	{
		c = clList->current->buf->str[++(*i)];
	}
	return c;
}



/* */
void DivisionWords (struct clientlist * clList)
{
	char * parsedStr;
	char c;
	int i, j, cnt;
	
	cnt = clList->current->buf->cnt;
	parsedStr = (char *) malloc (cnt * sizeof(char));
	
	i = 0;
	c = clList->current->buf->str[0];
	c = SkipSpaces (clList, c, &i);
	while ( c != '\n' &&  c != '\0' && i <= cnt )
	{
		j = 0;
		c = SkipSpaces (clList, c, &i);
		while (c != '\0' && c != ' ' && c != '\t' && c != '\n'
			&& i <= cnt) 
		{
			parsedStr[j++] = c;
			parsedStr[j] = '\0';
			c = clList->current->buf->str[++i];
		}

		c = SkipSpaces (clList, c, &i);
		if ( c != '\0' || j != 1 )
		{
			AddWordToStructure (clList, parsedStr, j);
		}
	}

	free (parsedStr);
}


/* */
void ParseCommand (struct banker * bank)
{
	char * str;
	int i;
	int fd;
	struct client * user;

	user = bank->clList->current;
	fd = user->contact->fd;
	i = user->buf->cnt;
	str = user->buf->str;
	if ( str[i - 1] == '\n' )
	{
	
		DivisionWords (bank->clList);	

		FindCommand (bank);

		free (user->buf->str);
		free (user->buf);
		user->buf = (struct buffer*) malloc (sizeof(struct buffer));

		InitBuffer (user->buf);		

		ClearCommand (user->cmd);
		InitCommand (user->cmd);
		
		PrintComing (fd, bank->clList->statusStartGame);
	}
}

