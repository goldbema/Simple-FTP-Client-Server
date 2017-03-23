/*******************************************************************************
*      Filename: validate.c
*        Author: Maxwell Goldberg
* Last Modified: 3.11.17
*   Description: Contains a utility function for validating ftserver command 
*                line arguments.
*******************************************************************************/

#include "validate.h"

/*******************************************************************************
*      Function: validateArgs()
*   Description: Validates the command line port argument.
*    Parameters: int argc - The number of command line arguments.
*                char **argv - The command line arguments list.
* Preconditions: None.
*       Returns: The validated port string.
*******************************************************************************/

const char *validateArgs(int argc, char **argv) {
    int result = 0;
    int i;
    const char* port;
   
    /* Ensure that there are exactly 2 arguments. */ 
    if (argc == 1 || argc > 2) {
        fprintf(stderr, "ftserver: usage: ftserver <SERVER_PORT>\n");
        exit(1);
    }

    port = argv[1];

    /* Ensure that the port contains only digit characters. Obtain an 
       integer version of the port. */
    for (i = 0; i < strlen(port); i++) {
        result *= 10;
        if (isdigit(port[i])) {
            result += port[i] - '0';
        } else {
            fprintf(stderr, "ftserver: port must contain only digits\n");
            exit(1);
        }
    } 
    
    /* Validate the port number range */
    if (result < MIN_PORT || result > MAX_PORT) {
        fprintf(stderr, "ftserver: invalid port number\n");
        exit(1);
    }
    /* Remove leading zeroes from the valid port number */
    snprintf(argv[1], 6, "%d", result);

    return port;
}
