#include "parse.h"



/* */
void FindCommandBeforeStart (struct client * user)
{
	char * str;
	int fd;

	fd = user->fd;
	str = user->cmd->first->str;

	if ( user->cmd->first != NULL )
	{
		GameNotStarted (fd);
	}	
}



/* */
void FindCommandAfterStart (struct client * user)
{
	char * str;
	int i;
	int fd;

	fd = user->fd;
	str = user->buf->str;
	i = user->buf->cnt - 2;

	if ( !strncmp (str, "MARKET", i) && i )
	{
		;
	}
	else if ( !strncmp (str, "PLAYER", i) && i )
	{
		;
	}
	else if ( !strncmp (str, "PROD", i) && i )
	{
		;
	}
	else if ( !strncmp (str, "BUY", i) && i )
	{
		;
	}
	else if ( !strncmp (str, "SELL", i) && i )
	{
		;
	}
	else if ( !strncmp (str, "BUILD", i) && i )
	{
		;
	}
	else if ( !strncmp (str, "TURN", i) && i )
	{
		;
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
	else if ( !strncmp(str, "EXIT", i) && i)
	{
		printf ("Now exit\n");	
		user->fExit = 1;
	}
	else if ( !strncmp(str, "HELP", i) && i)
	{
		Help (fd);
	}
	else if ( !strncmp(str, "", i) && (i == 0) )
	{
	}
	else 
		PrintHelp (fd);
}



/* */
void FindCommand (struct clientlist * clList)
{
	if (clList->statusStartGame == 1)
		FindCommandAfterStart (clList->current);	
	else
		FindCommandBeforeStart (clList->current);
}



/* */
void AddWordToStructure (struct clientlist * clList, char * str, int i)
{
	struct word * word;
	word = (struct word * ) malloc (sizeof(struct word));
	word->str = (char *) malloc (i * sizeof(char));
	word->next = NULL;

	strcpy (word->str, str);

	if ( clList->current->cmd->first == NULL )
		clList->current->cmd->first = word;
	else
		clList->current->cmd->last->next = word;	
	
	clList->current->cmd->last = word;	
}


/* */
char SkipSpaces (char c)
{
	while ( c == '\0' || c == '\t' || c == ' ' )
	{
		c = getchar ();	
	}
	
	return c;
}



/* */
void DivisionWords (struct clientlist * clList)
{
	char * parsedStr;
	char c;
	int i, cnt;
	
	cnt = clList->current->buf->cnt;
	parsedStr = (char *) malloc (cnt * sizeof(char));

	SkipSpaces (c = getchar());
	while ( c != '\0' )
	{
		i = 0;
		SkipSpaces (c);
		while ( c != '\0' || c != ' ' || c != '\t' )
		{
			parsedStr[i++] = c;
			parsedStr[i] = '\0';
		}

		AddWordToStructure (clList, parsedStr, i);
		
	}

	free (parsedStr);
}


/* */
void ParseCommand (struct clientlist * clList)
{
	char * str;
	int i;
	struct client * user;

	user = clList->current;
	i = user->buf->cnt;
	str = user->buf->str;
	if ( str[i - 1] == '\n' )
	{
	
		//DivisionWords (clList);	

		FindCommand (clList);

		free (user->buf);
		user->buf = (struct buffer*) malloc (sizeof(struct buffer));

		InitBuffer (user->buf);		
	}
}

