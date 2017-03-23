/*******************************************************************************
*      Filename: ftserver.c
*        Author: Maxwell Goldberg
* Last Modified: 03.11.17
*   Description: The main ftserver function.
*******************************************************************************/

#include "validate.h"
#include "signal.h"
#include "socket.h"
#include "command.h"
#include "dyn_buffer.h"

#define NUM_CONNS 1

void closeWithErrorCheck(int);

/*******************************************************************************
*      Function: main()
*   Description: The main ftserver function.
*    Parameters: int argc - The argument count.
*                char **argv - The argument list.
* Preconditions: None.
*       Returns: 0 on success.
*******************************************************************************/

int main(int argc, char **argv) {
    const char *serverPort;             /* Server port string */
    int servFD, ctrlFD, dataFD;         /* Socket file descriptors */
    int status;                         /* Error variable */
    int skipFlag;                       /* Loop flag */

    struct sockaddr_storage clientAddr;             /* Client address info */
    socklen_t clientAddrSize = sizeof(clientAddr);  

    struct ClientCmd cmd = {0};         /* Client command info */

    char retMode;                       /* Reply mode */
    char host[1024];                    /* Client hostname */
    char inetAddr[INET_ADDRSTRLEN];     /* Client IP address */
    char dataPort[6];                   /* Client data port */

    struct DynBuf outBuffer;            /* Output buffer */

    /* Validate command line arguments to acquire the server port */
    serverPort = validateArgs(argc, argv);   
    /* Initialize the server */
    servFD = initServer(serverPort);    
    /* Register the signal handler */
    registerHandler();

    /* Listen for inbound connections */
    if (listen(servFD, NUM_CONNS) == -1) {
        perror("ftserver: listen");
        exit(2);
    }

    printf("Server open on %s\n", serverPort);

    while(1) {

        skipFlag = 0;

        printf("----------------------\n");
        printf("Waiting on inbound connection...\n");

        /* Accept an inbound connection */
        ctrlFD = accept(servFD, (struct sockaddr *)&clientAddr,
                       &clientAddrSize);
        if (ctrlFD == -1) {
            perror("ftserver: accept");
            skipFlag = 1;
        }
        /* Get the client hostname and IP */ 
        if (!skipFlag) {
            obtainClientCredentials(&clientAddr, host, inetAddr);

            printf("----------------------\n");

            printf("Connection from %s\n", host);
        }
        /* Get the client command */
        if (!skipFlag && cmdRecv(ctrlFD, &cmd) <= 0) {
            closeWithErrorCheck(ctrlFD);
            skipFlag = 1;
        }
        /* Get the data port in string form */ 
        if (!skipFlag) { 
            memset(dataPort, 0, sizeof(dataPort));
            sprintf(dataPort, "%d", cmd.dataPort);
        }

        if (!skipFlag) {
            /* Output user requested action */
            printClientReq(&cmd);

            /* Generate return message body */
            initDynBuf(&outBuffer); 
            retMode = handleCmd(&cmd, &outBuffer, host, serverPort);

            /* Send an error message if an error occurred on the ctrl conn */
            if (retMode == 'e') { 
                cmd.dataPort = strtol(serverPort, NULL, 10);
                if (responseSend(ctrlFD, retMode, &outBuffer, &cmd)) {
                    closeWithErrorCheck(ctrlFD);
                }
            /* Otherwise, send the client the requested info */
            } else if (retMode == 'r') {
                /* Initialize the data connection */
                dataFD = initDataConn(inetAddr, dataPort);
                if (dataFD != -1) {
                    /* Send the response */
                    if (responseSend(dataFD, retMode, &outBuffer, &cmd)) {
                        closeWithErrorCheck(dataFD);
                        closeWithErrorCheck(ctrlFD); 
                    }
                }
            }
            freeDynBuf(&outBuffer);
        }
    }

    return 0;
}

/*******************************************************************************
*      Function: closeWithErrorCheck()
*   Description: Attempts to close a socket, displaying any errors.
*    Parameters: int sockfd - The socket file descriptor.
* Preconditions: None.
*       Returns: None.
*******************************************************************************/

void closeWithErrorCheck(int sockfd) {
    if (close(sockfd) != 0) {
        perror("ftserver: close");
    }
}
