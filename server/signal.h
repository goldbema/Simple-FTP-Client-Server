/*******************************************************************************
*      Filename: signal.h
*        Author: Maxwell Goldberg
* Last Modified: 03.11.17
*   Description: The header file for signal.c. Please see signal.c for more 
*                details.
*******************************************************************************/

#ifndef SIGNAL_H
#define SIGNAL_H

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void catchSIGINT(int);
void registerHandler();

#endif
