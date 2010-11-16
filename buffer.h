#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "main.h"

#define BUF_SIZE 32

struct buffer 
{
	char *str;
	int cnt;
	int part;
};

#endif
