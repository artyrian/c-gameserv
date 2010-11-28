#include "buffer.h"



/* */
void InitBuffer (struct buffer * buf)
{
	buf->str = (char *) malloc (BUF_SIZE);
	buf->part = 1;
	buf->cnt = 0;
}




/* Extend buffer (linear) */
void ExtendBuffer (struct buffer *buf)
{
	char * tmp_str;

	tmp_str = malloc ((++buf->part) * BUF_SIZE);
	strcpy (tmp_str, buf->str);

	free (buf->str);

	buf->str = tmp_str;
}
