#include "parse.h"
#include "server.h"
#include "commands.h"
#include "client.h"
#include "print.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


void FindCommandBeforeStart (struct client *);
void FindInfoCommand(struct banker *);
int FindActionCommand (struct banker *);
void FindCommand (struct banker *);
void AddWordToStructure (struct clientlist *, char *, int);
char SkipSpaces (struct clientlist *, char, int *i);
void DivisionWords (struct clientlist *);
void ParseCommand (struct banker *);



/* Compare user->cmd->first with cmd in fn.
 * if can't find, print help to help :)
 */
void FindCommandBeforeStart (struct client * user)
{
	char * str;
	int i;
	int fd;
	
	fd = user->contact->fd;
	str = user->cmd->first->str;
	i = strlen (user->cmd->first->str) - 1;

	if ( strncmp (str, "help", 4) == 0 )
	{
		Help (fd);
	}
	else
	{
		PrintGameNotStarted (fd);
	}

}




/* Compare user->cmd->first with cmd in fn.
 * if can't find, print help to help :)
 */
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

	if ( strncmp (str, "market", 6) == 0 )
	{
		Market (bank);	
	}
	else if ( strncmp (str, "player", 6) == 0 )
	{
		PlayerInfo (bank);	
	}
	else if ( strncmp (str, "whoami", 6) == 0 )
	{
		WhoAmI (bank);
	}
	else if ( strncmp(str, "help", 4) == 0 )
	{
		Help (fd);
	}
	else 
		PrintHelp (bank);
}



/* Compare user->cmd->first with cmd in fn.
 * if can't find, print help to help :)
 */
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

	if ( !strncmp (str, "prod", 4) && i)
	{
		Production (bank);		
	}
	else if ( !strncmp (str, "buy", 3) && i )
	{
		Buy (bank);	
	}
	else if ( !strncmp (str, "sell", 4) && i )
	{
		Sell (bank);	
	}
	else if ( !strncmp (str, "build", 5) && i )
	{
		Build (bank);	
	}
	else if ( !strncmp (str, "turn", 4) && i )
	{
		TurnStep (bank);
	}
	else 
		result = 0;	
	return result;
}




/* 
 *
 */
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
		else if ( FindActionCommand (bank) == 0 )
		{
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



/* Skip tabs, spaces and enters
 */
char SkipSpaces (struct clientlist * clList, char c, int * i)
{
	while ( c == '\t' || c == ' ' || c == '\n' ) 
	{
		c = clList->current->buf->str[++(*i)];
	}
	return c;
}



/* 
 * SEE IT!!! 100% it's make simpleer.
 */
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
	while ( c != '\0' && i <= cnt )
	{
		j = 0;
		c = SkipSpaces (clList, c, &i);
		while (c != '\0' && c != ' ' && c != '\t' && c != '\n')
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



/* Use data in buffer: divisions by words and find such cmd 
 * Reset,  init buffer; refresh cmd, print coming msg.
 */
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

		FreeBuffer (user->buf);
		user->buf = (struct buffer*) malloc (sizeof(struct buffer));

		InitBuffer (user->buf);		

		ClearCommand (user->cmd);
		InitCommand (user->cmd);
		
		PrintComing (fd, bank->clList->statusStartGame);
	}
}
