#include "parse.h"



/* */
void FindCommand (struct buffer * buf)
{
	char * str;
	int i;

	const char strHelp[160] = "Call this help you can with command HELP\n To incremente global variable use INC\n To watch value use PRINT\n To exit from game use EXIT.\0";
	
	str = buf->str;
	i = buf->cnt - 2;

	if ( !strncmp (str, "INC", i) )
		printf ("Now inc\n");	
	else if ( !strncmp(str, "PRINT", i) )
		printf ("Now print\n");	
	else if ( !strncmp(str, "EXIT", i) )
		printf ("Now exit\n");	
	else 
		printf ("%s\n", strHelp);	
}



/* */
void ParseCommand (struct buffer * buf, int rc)
{
	char *str;
	int i;

	i = buf->cnt;
	str = buf->str;
	if ( str[i - 1] == '\n' )
	{
		FindCommand (buf);
		free (buf);
		buf = (struct buffer*) malloc (sizeof(struct buffer));

		InitBuffer (buf);		
	}
}

