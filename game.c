#include "game.h"



/* */
void InitBank (struct banker * bank, char ** argv)
{
 	CreateClientList ((&bank->clList), argv);
}



/* */
void CreateBank (struct banker ** bank, char ** argv)
{
	*bank = (struct banker *) malloc (sizeof(struct banker));
	InitBank (*bank, argv);
}



