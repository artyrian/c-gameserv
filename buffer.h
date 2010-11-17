#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "main.h"
#include "client.h"

#define BUF_SIZE 4 
#define ON 1

struct buffer 
{
	char *str;
	int cnt;
	int part;
};

void InitBuffer (struct buffer *);
void ExtendBuffer (struct buffer *);

#endif

