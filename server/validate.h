/*******************************************************************************
*      Filename: validate.h
*        Author: Maxwell Goldberg
* Last Modified: 3.11.17
*   Description: The header file for validate.c. Please see validate.c for more
*                details.
*******************************************************************************/

#ifndef VALIDATE_H
#define VALIDATE_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_PORT   1
#define MAX_PORT   65535
#define MSGBUFSIZE 256

const char *validateArgs(int, char **);

#endif
