#include "serverinfo.h"

/* get number of players from argv*/
int getNumberPlayers  (char ** argv)
{
	int num = 0;
	
	if ( argv[1] != '\0' )
		num = atoi (argv[1]);
	return num;
}



/* */
int getPort (char ** argv)
{
	int port = 0;

	if ( argv[2] != '\0' )
		port = atoi (argv[2]);
	return port;
}



/* */
void printServerInfo (int port, int numPl)
{
	printf ("Max. players:%d\n", numPl);
	printf ("Port:%d.\n", port);
}



/* */
void printDescriptors (int *arr_d, int cur_num)
{
	int i;
	
	printf ("Current number of players:%d\n", cur_num);
	printf ("List of descriptors:\n");
	for ( i = 0; i < cur_num; i++ ) 
		printf ("fd=%d.", arr_d[i]);
	printf ("\n");
}
