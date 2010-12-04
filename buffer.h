#ifndef _BUFFER_H_
#define _BUFFER_H_


#define BUF_SIZE 32 


struct buffer 
{
	char *str;
	int cnt;
	int part;
};


void InitBuffer (struct buffer *);
void ExtendBuffer (struct buffer *);
void FreeBuffer (struct buffer *);


#endif

