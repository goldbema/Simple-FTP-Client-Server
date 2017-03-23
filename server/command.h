/*******************************************************************************
*      Filename: command.h
*        Author: Maxwell Goldberg
* Last Modified: 03.11.17
*   Description: The header file for command.c. Please see command.c for more
*                details.
*******************************************************************************/

#ifndef COMMAND_H
#define COMMAND_H

#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dyn_buffer.h"

#define FNAME_MAX 255   /* Maximum filename length in bytes */
#define HEADER_LEN 7    /* Application level header length */

/* Struct representing unpacked client command values */
struct ClientCmd {
    unsigned int dataPort; /* Port to be used to make the data connection */
    unsigned int len;      /* Length of the data segment to follow the header */
    char mode;             /* Command mode */
    char fName[FNAME_MAX]; /* Requested file name */
};

void processHeader(char *, struct ClientCmd *);
void packHeader(struct ClientCmd *, char, char *, char *, int);
char handleCmd(struct ClientCmd *, struct DynBuf *, const char *, const char *);

void printClientReq(struct ClientCmd *);

#endif
