#include "commands.h"



/* */
void InitCommand (struct command * cmd)
{
	cmd->first = cmd->last = NULL;	
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

}



/* */
void PlayerInfo (struct banker * bank)
{

}



/* */
void Production (struct banker * bank)
{

}


/* */
void Buy (struct banker * bank)
{

}



/* */
void Sell (struct banker * bank)
{
}

void Build (struct banker * bank)
{

}



/* */
void TurnStep (struct banker * bank)
{

}



/* */
void Help (int fd)
{
	const char strHelp[NUMBER_OF_COMMANDS * MESSAGE_LENGHT] = 
	"It's HELP to commands. Write:\nHELP - to call this help.\nINC - to increment global i.\n\0"
	;
	
	write (fd, &strHelp, strlen(strHelp) + 1);
}
/* */
void PrintHelp (int fd)
{
	const char strHelp[MESSAGE_LENGHT] =
	"Call help you can with command HELP\n\0";
	
	write (fd, &strHelp, strlen(strHelp) + 1);
}
