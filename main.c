#include "main.h"

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
		int max_d; 

		FD_ZERO (&readfds);
		FD_SET (ls, &readfds);

		max_d = MaxDescriptor (clList, &readfds, ls);

	 	CheckActionOnFD_SET (max_d, &readfds);

		if (FD_ISSET(ls, &readfds))
			AcceptQuery (ls, clList);

		CheckDataFromClients (clList, &readfds);
	}

	free (clList);

	return 0;
}

