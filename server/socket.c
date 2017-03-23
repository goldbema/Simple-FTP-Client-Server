/*******************************************************************************
*      Filename: socket.c
*        Author: Maxwell Goldberg
* Last Modified: 03.11.17
*   Description: Contains utility functions for initializing a listening socket,
*                obtaining address information, initializing a connection, as well
*                as sending and receiving data.
*******************************************************************************/

#include "socket.h"

/*******************************************************************************
*      Function: obtainClientCredentials()
*   Description: Obtains the hostname and IP address if the socket identified
*                by a struct sockaddr_storage.
*    Parameters: struct sockaddr_storage *clientAddr - The sockaddr struct.
*                char *host - The destination of the hostname.
*                char *inetStr - The destination of the IP address.
* Preconditions: The sockaddr_storage * points to the socket information.
*       Returns: 0 on success, 1 on failure to find the hostname.
*******************************************************************************/

int obtainClientCredentials(struct sockaddr_storage *clientAddr, char *host,
                         char *inetStr) {
    struct addrinfo hints, *targetinfo;
    struct hostent *he;
    int status;

    /* Get the presentation internet address */
    inet_ntop(AF_INET, &(((struct sockaddr_in *) clientAddr)->sin_addr), 
              inetStr, INET_ADDRSTRLEN);
    /* Get the host information. Note that we use the deprecated 
     * gethostbyaddr() here because getnameinfo() produced
     * a memory leak (or the appearance of a leak in Valgrind). */ 
    he = gethostbyaddr(&(((struct sockaddr_in *) clientAddr)->sin_addr),
                       INET_ADDRSTRLEN, AF_INET);
    if (!he) {
        herror("ftserver: gethostbyname");
        strcpy(host, "unknown host"); 
        return 1;
    } 

    strcpy(host, he->h_name);
    return 0;
}

/*******************************************************************************
*      Function: _obtainAddress()
*   Description: Returns a struct addrinfo corresponding to hostname and port
*                strings passed as arguments.
*    Parameters: const char *hName - The hostname string.
*                const char *serverPort - The server port string.
* Preconditions: None.
*       Returns: The struct addrinfo pointer.
*******************************************************************************/

struct addrinfo *_obtainAddress(const char *hName, const char *serverPort) {
    struct addrinfo hints, *servinfo;
    int resultVal;
    /* Form the hints addrinfo */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;       /* Either IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;   /* TCP socket */
    hints.ai_flags = AI_PASSIVE;       /* Use localhost by default */

    resultVal = getaddrinfo(hName, serverPort, &hints, &servinfo);
    if (resultVal != 0) {
        fprintf(stderr, "ftserver: getaddrinfo: %s\n", gai_strerror(resultVal));
        exit(2);
    }    
    
    return servinfo;
}

/*******************************************************************************
*      Function: _bindSocket()
*   Description: Attempts to initialize and bind a socket based on information
*                gathered by getaddrinfo().
*    Parameters: struct addrinfo *servinfo - A pointer to the server addr info.
* Preconditions: servinfo points to the server information.
*       Returns: The socket file descriptor.
*******************************************************************************/

int _bindSocket(struct addrinfo *servinfo) {
    int sockFD;
    int boolean = 1;
    struct addrinfo *p;

    /* Go through all possible addrinfo structs */
    for (p = servinfo; p != NULL; p = p->ai_next) {
        /* Open a socket as specified by the struct */
        sockFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockFD == -1) {
            perror("ftserver: socket");
            continue;
        }
        /* Make it so that the socket can be immediately reused. */
        if (setsockopt(sockFD, SOL_SOCKET, SO_REUSEADDR, &boolean, 
                       sizeof(int)) == -1) {
            perror("ftserver: setsockopt");
            exit(2);
        }
        /* Attempt to bind the socket */
        if (bind(sockFD, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockFD);
            perror("ftserver: bind");
            continue;
        }
        break;
    }

    if (!p) {
        fprintf(stderr, "ftserver: failed to bind\n");
        exit(2);
    }
    /* Free allocated memory for the addrinfo struct */
    freeaddrinfo(servinfo);

    return sockFD; 
}

/*******************************************************************************
*      Function: _connectSocket()
*   Description: Attempts to initialize and connect a socket to a server 
*                specified by a struct addrinfo.
*    Parameters: struct addrinfo *servinfo - The server information.
* Preconditions: None.
*       Returns: The socket file descriptor, -1 on failure.
*******************************************************************************/

int _connectSocket(struct addrinfo *servinfo) {
    int sockFD;
    struct addrinfo *p;

    /* Iterate through all possible addrinfos. */
    for (p = servinfo; p != NULL; p = p->ai_next) {
        /* Initialize a socket */
        sockFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockFD == -1) {
            perror("ftserver: socket");
            continue;
        }
        /* Attempt to connect */
        if (connect(sockFD, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockFD);
            perror("ftserver: connect");
            continue; 
        }

        break;
    }

    if (!p) {
        fprintf(stderr, "ftserver: failed to bind\n");
        return -1;
    }
    /* Free the addrinfo struct */
    freeaddrinfo(servinfo);

    return sockFD;    
}

/*******************************************************************************
*      Function: initServer()
*   Description: Obtain the server address info and initailize the server
*                socket.
*    Parameters: const char *serverPort - The server listening port.
* Preconditions: None.
*       Returns: The server socket file descriptor.
*******************************************************************************/

int initServer(const char *serverPort) {
    struct addrinfo *servinfo;
    
    servinfo = _obtainAddress(NULL, serverPort);
    return _bindSocket(servinfo);
}

/*******************************************************************************
*      Function: initDataConn()
*   Description: Obtain the client's listening socket address and attempt to
*                connect to it.
*    Parameters: const char *hostInfo - The client IP address info string.
*                const char *dataPort - The client data listening port.
* Preconditions: None.
*       Returns: The data connection file descriptor, -1 on failure.
*******************************************************************************/

int initDataConn(const char *hostInfo, const char *dataPort) {
    struct addrinfo *servinfo;

    servinfo = _obtainAddress(hostInfo, dataPort);
    return _connectSocket(servinfo); 
}

/*******************************************************************************
*      Function: sendAll()
*   Description: Attempts to send all bytes of a message into a socket.
*    Parameters: int sockfd - The socket file descriptor.
*                char *msg - The byte string to be sent.
*                int msgLen - The length of the string.
* Preconditions: None.
*       Returns: 0 on success, 1 on failure.
*******************************************************************************/

int sendAll(int sockfd, char *msg, int msgLen) {
    int totalSent = 0;
    int currSent;

    /* Continue sending while the total number of bytes sent is less than
     * the message length */
    while (totalSent < msgLen) {
        currSent = send(sockfd, &msg[totalSent], msgLen-totalSent, 0);
        if (currSent == -1) {
            perror("ftserver: send");
            return 1;
        }

        totalSent += currSent;
    }

    return 0;
}

/*******************************************************************************
*      Function: _cmdRecvHelper()
*   Description: Attempts to take in msgLen bytes of a sending socket's message.
*    Parameters: int sockfd - The socket file descriptor.
*                char *msg - The message buffer.
*                int msgLen - The length of the message to be received.
* Preconditions: None.
*       Returns: 0 on sender closure, -1 on error, a positive value otherwise.
*******************************************************************************/

int _cmdRecvHelper(int sockfd, char *msg, int msgLen) {
    char buffer[msgLen+1];
    int bytesRecvd = 0;
    int status, i;

    /* While part of the message (up to msgLen) hasn't been received... */
    while(bytesRecvd < msgLen) {
        memset(buffer, 0, sizeof(buffer));

        /* Receive more of the message */
        status = recv(sockfd, buffer, msgLen - bytesRecvd, 0);
   
        /* Handle socket closure */
        if(status == 0) {
            fprintf(stderr, "ftserver: Client ended connection.\n");
            if (close(sockfd) != 0) {
                perror("ftserver: close");
            }
            return status;
        }
        /* Handle errors */
        if(status == -1) {
            perror("ftserver: recv");
            if (close(sockfd) != 0) {
                perror("ftserver: close");
            }
            return status;
        }
        /* Append the temp buffer to the message buffer */ 
        for (i =0; i < status; i++) {
            msg[bytesRecvd + i] = buffer[i];
        }
 
        bytesRecvd += status;
    }

    return status;
}

/*******************************************************************************
*      Function: cmdRecv()
*   Description: Receives an entire client command.
*    Parameters: int sockfd - The socket file descriptor.
*                struct ClientCmd *cmd - The client command struct to inform.
* Preconditions: None.
*       Returns: -1 on recv() error, 0 on unexpected socket closure, and a
*                positive value otherwise.
*******************************************************************************/

int cmdRecv(int sockfd, struct ClientCmd *cmd) {
    char header[HEADER_LEN+1];
    char body[FNAME_MAX+1];
    int status;

    memset(header, 0, sizeof(header));
    memset(body, 0, sizeof(body));

    /* Get the header byte string */
    status = _cmdRecvHelper(sockfd, header, HEADER_LEN);

    if (status <= 0) {
        return status;
    }
    
    /* Process the header into the struct */ 
    processHeader(header, cmd);    

    /* Get the body byte string */
    if (cmd->len) {
        status = _cmdRecvHelper(sockfd, body, cmd->len);
        if (status <= 0) {
            return status;
        }
        strcpy(cmd->fName, body);
    }
    return status;
}

/*******************************************************************************
*      Function: responseSend()
*   Description: Sends an entire response message into a socket.
*    Parameters: int sockfd - The socket file descriptor.
*                char mode - The response mode.
*                struct DynBuf *body - The dynamic string.
*                struct ClientCmd *cmd - The client command.
* Preconditions: The client command, mode, and message body have been 
*                initialized.
*       Returns: 0 on success, 1 on failure.
*******************************************************************************/

int responseSend(int sockfd, char mode, struct DynBuf *body, 
                  struct ClientCmd *cmd) {
    char header[HEADER_LEN+1];
    char *msg;
    int i, status;

    memset(header, 0, sizeof(header));

    /* Pack the header according to the client command */
    packHeader(cmd, mode, header, body->buffer, body->size);

    /* Pack the message into a single string */
    msg = malloc(sizeof(char) * (HEADER_LEN + body->size + 1));
    memset(msg, 0, sizeof(msg));

    for (i = 0; i < HEADER_LEN; i++) {
        msg[i] = (unsigned char) header[i];
    }

    for (i = 0; i < body->size; i++) {
        msg[i+HEADER_LEN] = body->buffer[i];
    }
    /* Send the message */
    status = sendAll(sockfd, msg, HEADER_LEN + body->size);   

    free(msg); 

    return status;
}
