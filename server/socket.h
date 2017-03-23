/*******************************************************************************
*      Filename: socket.h
*        Author: Maxwell Goldberg
* Last Modified: 03.11.17
*   Description: The header file for socket.c Please see socket.c for more 
*                details.
*******************************************************************************/

#ifndef SOCKET_H
#define SOCKET_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "command.h"

int initServer(const char *);
int initDataConn(const char *, const char *);
int sendAll(int, char *, int);
int cmdRecv(int, struct ClientCmd *);
int responseSend(int, char, struct DynBuf *, struct ClientCmd *);

int obtainClientCredentials(struct sockaddr_storage *, char *, char *);

#endif
