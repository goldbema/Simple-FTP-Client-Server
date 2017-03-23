/*******************************************************************************
*      Filename: dyn_buffer.h
*        Author: Maxwell Goldberg
* Last Modified: 03.11.17
*   Description: The header file for dyn_buffer.c. Please see dyn_buffer.c for 
*                more details.
*******************************************************************************/

#ifndef DYN_BUFFER_H
#define DYN_BUFFER_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define DB_START_LEN     256    /* Dynamic buffer starting length */
#define DB_RESIZE_FACTOR   2    /* Dynamic resizing factor */

struct DynBuf {
    int size;
    int cap;
    char *buffer;
};

void initDynBuf(struct DynBuf *);
void dynBufAdd(struct DynBuf *, char);
void dynBufAddStr(struct DynBuf *, const char *);
void clearDynBuf(struct DynBuf *);
void freeDynBuf(struct DynBuf *);

#endif
