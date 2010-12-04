#include "buffer.h"
#include <stdlib.h>
#include <string.h>



void InitBuffer (struct buffer *);
void ExtendBuffer (struct buffer *);
void FreeBuffer (struct buffer *);


/* Initialization buffer.
 * Create string for write
 * and init cnts of parts(1) and symbols(0)
 */
void InitBuffer (struct buffer * buf)
{
	buf->str = (char *) malloc (BUF_SIZE);
	buf->part = 1;
	buf->cnt = 0;
}




/* Extend buffer (linear).
 * Inc part of buf, 
 * create new string greater than was, 
 * copy old to new
 * delete old
 */
void ExtendBuffer (struct buffer *buf)
{
	char * tmp_str;

	tmp_str = malloc ((++buf->part) * BUF_SIZE);
	strcpy (tmp_str, buf->str);

	free (buf->str);

	buf->str = tmp_str;
}



/* Free buffer
 */
 void FreeBuffer (struct buffer * buf)
 {
	free (buf->str);
	free (buf);
 }
