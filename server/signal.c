/*******************************************************************************
*      Filename: signal.c
*        Author: Maxwell Goldberg
* Last Modified: 03.11.17
*   Description: The SIGINT signal handler and a utility for registering it in
*                the main function.
*******************************************************************************/

#include "signal.h"

/*******************************************************************************
*      Function: catchSIGINT()
*   Description: The SIGINT signal handler.
*    Parameters: int signo - The SIGINT signal handler.
* Preconditions: None.
*       Returns: None.
*******************************************************************************/

void catchSIGINT(int signo) {
    /* Notify the user that the server is exiting. Clean up of the data and
     * control sockets is performed in the main server loop */
    write(1, "Exiting ftserver.\n", 18);
    exit(0);
}

/*******************************************************************************
*      Function: registerHandler()
*   Description: Registers the SIGINT signal handler.
*    Parameters: None.
* Preconditions: None.
*       Returns: None.
*******************************************************************************/

void registerHandler() {
    struct sigaction sa;

    /* Set the signal handler */
    sa.sa_handler = catchSIGINT;
    sa.sa_flags = 0;
    /* Block other SIGINT signals during the SIGINT handler */
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);

    /* Register the handler */
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("ftserver: sigaction");
        exit(1);
    }
}
