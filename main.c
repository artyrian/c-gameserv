#include "main.h"
#include "server.h"
#include "client.h"

int main(int argc, char ** argv, char ** envp)
{
	int ls; 
	struct clientlist *clList;
	struct sockaddr_in addr;
	
 	CreateClientList (&clList, argv);

 	InitSockaddr (&addr, argv);

	CreateListenSocket (&ls);

	BindAddress (ls, &addr);

	ListeningState (ls);

	for (;;)
	{ 
		fd_set readfds; 

		FD_ZERO (&readfds);
		FD_SET (ls, &readfds);

	 	CheckActionOnFD_SET (clList, ls, &readfds);

		if (FD_ISSET(ls, &readfds))
			AcceptQuery (ls, clList);

		CheckDataFromClients (clList, &readfds);
	}

	free (clList);

	return 0;
}
