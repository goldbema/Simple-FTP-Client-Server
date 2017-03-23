/*******************************************************************************
*      Filename: dyn_buffer.c
*        Author: Maxwell Goldberg
* Last Modified: 03.11.17
*   Description: Functions for initializing, deallocating, and adding to a 
*                dynamic array data structure that holds character data,
*******************************************************************************/

#include "dyn_buffer.h"

/*******************************************************************************
*      Function: initDynBuf()
*   Description: Initializes a DynBuf struct.
*    Parameters: struct DynBuf *db - A pointer to the struct to initialize.
* Preconditions: None.
*       Returns: None.
*******************************************************************************/

void initDynBuf(struct DynBuf *db) {
    assert(db);

    db->size = 0;
    db->cap = DB_START_LEN;
    db->buffer = malloc(sizeof(char) * DB_START_LEN);
    assert(db->buffer);

    memset(db->buffer, 0, sizeof(db->buffer));
}

/*******************************************************************************
*      Function: _resizeBuf()
*   Description: Resizes a DynBuf struct buffer.
*    Parameters: struct DynBuf *db - A pointer to the struct to resize.
* Preconditions: The DynBuf has been initialized.
*       Returns: None.
*******************************************************************************/

void _resizeBuf(struct DynBuf *db) {
    assert(db);
    assert(db->cap > 0);

    /* Allocate a new buffer */ 
    int i;
    char *temp = malloc(sizeof(char) * db->cap * DB_RESIZE_FACTOR);
    assert(temp);
    memset(temp, 0, sizeof(temp));
 
    /* Copy existing data to the new buffer */
    for (i = 0; i < db->size; i++) {
        temp[i] = db->buffer[i];
    } 
    
    /* Free the old buffer and replace it with the new buffer */
    free(db->buffer);
    db->buffer = temp; 
    db->cap *= DB_RESIZE_FACTOR;
}

/*******************************************************************************
*      Function: dynBufAdd()
*   Description: Adds a single character to the end of the dynamic buffer.
*    Parameters: struct DynBuf *db - A pointer to the struct.
*                char c - The character value to append to the buffer.
* Preconditions: The DynBuf has been initialized.
*       Returns: None.
*******************************************************************************/

void dynBufAdd(struct DynBuf *db, char c) {
    assert(db);

    /* Resize if necessary */
    if (db->size  >= db->cap-1) {
        _resizeBuf(db);
    }

    /* Add the character and increment the size */
    db->buffer[db->size] = c;
    db->size++;
}

/*******************************************************************************
*      Function: dynBufAddStr()
*   Description: Appends a character string to the DynBuf.
*    Parameters: struct DynBuf *db - A pointer to the struct.
* Preconditions: The DynBuf has been initialized.
*       Returns: None.
*******************************************************************************/

void dynBufAddStr(struct DynBuf *db, const char *str) {
    assert(db);

    int i;
    int len = strlen(str);
    for (i = 0; i < len; i++) {
        dynBufAdd(db, str[i]);
    }
}

/*******************************************************************************
*      Function: clearDynBuf()
*   Description: Replaces all characters in the buffer with null terminators.
*    Parameters: struct DynBuf *db - A pointer to the struct.
* Preconditions: The DynBuf has been initialized.
*       Returns: None.
*******************************************************************************/

void clearDynBuf(struct DynBuf *db) {
    assert(db);

    memset(db->buffer, 0, sizeof(db->buffer));
    db->size = 0;
}

/*******************************************************************************
*      Function: freeDynBuf()
*   Description: Deallocates memory allocated within the struct. Note that if 
*                the struct is on the heap, it must be freed separately.
*    Parameters: struct DynBuf *db - A pointer to the struct.
* Preconditions: The DynBuf has been initialized.
*       Returns: None.
*******************************************************************************/

void freeDynBuf(struct DynBuf *db) {
    assert(db);
    assert(db->buffer);

    free(db->buffer);
    db->size = 0;
    db->cap = 0;
}
