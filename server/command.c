/*******************************************************************************
*      Filename: command.c
*        Author: Maxwell Goldberg
* Last Modified: 03.11.17
*   Description: Utilities for packing and unpacking application-layer headers,
*                as well as handling client commands on the server side.
*******************************************************************************/

#include <stdio.h>

#include "command.h"

/*******************************************************************************
*      Function: bytesToInt()
*   Description: Converts a byte string to its big-endian integer value.
*    Parameters: char *c - The byte string to be converted.
*                int len - The length (in bytes) of the string.
* Preconditions: The length of the string is in 1-4 inclusive. The byte string
*                is of length len.
*       Returns: The integer value.
*******************************************************************************/

int bytesToInt(char *c, int len) {
    assert(len > 0 && len <= 4);

    int result = 0;
    int i;

    /* Shift result values left by one byte and fill in the gap */
    for(i = 0; i < len; i++) {
        result <<= 8;
        result += (unsigned char) *(c + i);
    }

    return result;
}

/*******************************************************************************
*      Function: intToBytes()
*   Description: Converts an integer into a big-endian byte string.
*    Parameters: char *c - The byte string to hold the final string.
*                int wordSize - The number of elements in the byte string.
*                int val - The value to convert.
* Preconditions: c is able to hold wordSize elements. val is positive, and can
*                be contained in wordSize * 8 bytes.
*       Returns: None.
*******************************************************************************/

void intToBytes(char *c, int wordSize, int val) {
    assert(wordSize >0 && wordSize <= 4);

    int i, current;
    int mask = 255; /* All bits in the lowest byte are set */

    /* Obtain 8 bits by ANDing with the mask, and shift off the obtained
     * portion. */  
    for (i = wordSize-1; i >= 0; i--) {
         current = val & mask;
         *(c + i) = (unsigned char) current;
         val >>= 8;
    }
}

/*******************************************************************************
*      Function: processHeader()
*   Description: Unpacks a client command into a struct ClientCmd.
*    Parameters: char *hdr - The header byte string.
*                struct ClientCmd *cmd - The struct to hold the result.
* Preconditions: The struct has been initialized. The header byte string 
*                contains the client header.
*       Returns: None.
*******************************************************************************/

void processHeader(char *hdr, struct ClientCmd *cmd) {
    /* Get the mode. */
    cmd->mode = hdr[0];
    /* Get the data port number */
    cmd->dataPort = bytesToInt(&hdr[1], 2);
    /* Get the length of the application data body */
    cmd->len = bytesToInt(&hdr[3], 4);
}

/*******************************************************************************
*      Function: packHeader()
*   Description: Packs a header string.
*    Parameters: struct ClientCmd *cmd - The struct containing a client cmd.
*                char mode - The returning mode of the struct (r for a reply,
*                            or e for an error message).
*                char *header - The header string to be packed.
*                char *body - The data string reply.
* Preconditions: cmd contains a client command. Mode contains the reply mode.
*                header is a string long enough to contain the header. body
*                contains the application data to be sent back.
*       Returns: None.
*******************************************************************************/

void packHeader(struct ClientCmd *cmd, char mode, char *header, char *body,
                int bodyLen) {
    header[0] = mode;
    /* If the mode is a reply, return the data port number. This isn't strictly
     * necessary, but could be used for additional verification on the client 
     * side. */
    if (mode == 'r') {
        intToBytes(&header[1], 2, cmd->dataPort);
    }

    /* Pack the data length */
    intToBytes(&header[3], 4, bodyLen);
}

/*******************************************************************************
*      Function: generateList()
*   Description: Performs the '-l' mode user command by reading a list of 
*                regular files in the current directory into a DynBuf struct.
*    Parameters: struct DynBuf *msgBuf - The buffer to hold the list of files.
* Preconditions: msgBuf has been initialized.
*       Returns: 'r' if the command succeeds, 'e' otherwise.
*******************************************************************************/

char generateList(struct DynBuf *msgBuf) {
    DIR *dirp;
    struct dirent *ep;
    const char *dirName;
    int count = 0;
    
    /* Open the current directory */
    dirp = opendir("./");
    if (!dirp) {
        perror("ftserver: opendir");
        exit(3);
    } 

    /* Read every entry in the directory and add all regular file names to 
     * the DynBuf string */
    while (ep = readdir(dirp)) {
        dirName = ep->d_name;
        if (ep->d_type == DT_REG) {
            dynBufAddStr(msgBuf, dirName);
            dynBufAdd(msgBuf, '\n');
            count++;
        }
    }
   
    /* If we're unable to read anything into the buffer, exit with an error. */
    if (count == 0) {
        clearDynBuf(msgBuf);
        dynBufAddStr(msgBuf, "NO DIRECTORY CONTENTS");
        return 'e';
    }

    /* Close the directory */
    if (closedir(dirp) != 0) {
        perror("ftserver: closedir");
        exit(3);
    }

    return 'r';
}

/*******************************************************************************
*      Function: retrieveFile()
*   Description: Performs the '-g' mode user command by attempting to read a
*                file's contents into a byte string.
*    Parameters: struct DynBuf *msgBug - The buffer to hold the file data.
*                const char *fName - The file name.
* Preconditions: msgBuf has been initialized.
*       Returns: 'r' if the command succeeds, 'e' otherwise.
*******************************************************************************/

char retrieveFile(struct DynBuf *msgBuf, const char *fName) {
    FILE *fp;
    int i, len;
 
    /* Open the file for reading */
    fp = fopen(fName, "r");
    /* If the file can't be opened, return an error and place the error 
     * message in the buffer */
    if (!fp) {
        clearDynBuf(msgBuf);
        dynBufAddStr(msgBuf, "FILE NOT FOUND");
        return 'e';    
    }

    /* Seek to the end of the file, get the end position, and rewind to the
     * beginning of the file */
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    rewind(fp);  
  
    /* Add the file data to the buffer */ 
    for (i = 0; i < len; i++) {
        dynBufAdd(msgBuf, (unsigned char) fgetc(fp));
    }

    /* Close the file */
    if (fclose(fp) != 0) {
        perror("ftserver: fclose");
        exit(3);
    }
    fp = NULL;

    return 'r'; 
}

/*******************************************************************************
*      Function: handleCmd()
*   Description: Performs the client's requested command.
*    Parameters: struct ClientCmd *cmd - The client command struct.
*                struct DynBuf *msgBuf - The outgoing message buffer.
*                const char *clientHost - The client hostname.
*                const char *serverPort - The server listening port.
* Preconditions: The client hostname and server port are correct. The message
*                buffer has been initialized. The client command struct contains
*                the client command.
*       Returns: 'r' if the command succeeds, 'e' otherwise.
*******************************************************************************/

char handleCmd(struct ClientCmd *cmd, struct DynBuf *msgBuf, 
               const char *clientHost, const char *serverPort) {
    char returnMode;

    assert(cmd);
    assert(msgBuf);
    /* Process a 'get file' client request */
    if (cmd->mode == 'g') {
        returnMode = retrieveFile(msgBuf, cmd->fName);
        /* If the request succeeds, output this. */
        if (returnMode == 'r') {
            printf("Sending \"%s\" requested on port %d.\n", cmd->fName, 
                   cmd->dataPort);
        /* Otherwise, output failure */
        } else {
            printf("File not found. Sending error message to %s:%s.\n", 
                   clientHost, serverPort);
        }
    /* Process a 'list directory' request */
    } else if (cmd->mode == 'l') {
        returnMode = generateList(msgBuf);
        /* If the request succeeds, output this. */
        if (returnMode == 'r') {
            printf("Sending directory contents to %s:%d.\n", clientHost, 
                   cmd->dataPort);
        /* Otherwise, report failure */
        } else {
            printf("No directory contents. Sending error message to %s:%s.\n", 
                   clientHost, serverPort);
        }
    /* Process any other command as an error */
    } else {
        /* Add error text to the buffer */
        clearDynBuf(msgBuf);
	dynBufAddStr(msgBuf, "INVALID COMMAND");
        /* Set the response mode. */
        returnMode = 'e';
        /* Report failure */
        printf("Invalid command. Sending error message to %s:%s.\n",
               clientHost, serverPort);
    }
    return returnMode;
}

/*******************************************************************************
*      Function: printClientReq()
*   Description: Outputs the requested client command action.
*    Parameters: struct ClientCmd *cmd - The client command struct.
* Preconditions: The command struct contains the client command.
*       Returns: None.
*******************************************************************************/

void printClientReq(struct ClientCmd *cmd) {
    if (cmd->mode == 'g') {
        printf("File \"%s\" requested on port %d.\n", cmd->fName, 
                                                      cmd->dataPort); 
    } else if (cmd->mode == 'l') {
        printf("List directory requested on port %d.\n", cmd->dataPort);
    } else {
        printf("Unrecognized command requested on port %d.\n", cmd->dataPort);
    }
}
