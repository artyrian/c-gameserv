#include "main.h"
#include "server.h"
#include "client.h"
#include "game.h"

int main(int argc, char ** argv, char ** envp)
{
	int ls; 
	struct clientlist *clList;
	struct sockaddr_in addr;

	struct banker * bank;

	CreateBank (&bank, argv);

 	InitSockaddr (&addr, argv);

	CreateListenSocket (&ls);

	BindAddress (ls, &addr);

	ListeningState (ls);

	for (;;)
	{ 
		fd_set readfds; 

		FD_ZERO (&readfds);
		FD_SET (ls, &readfds);

	 	CheckActionOnFD_SET (bank->clList, ls, &readfds);

		if (FD_ISSET(ls, &readfds))
			AcceptQuery (ls, bank->clList);

		CheckDataFromClients (bank->clList, &readfds);
	}

	free (clList);

	return 0;
}
