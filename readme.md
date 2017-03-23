# Simple ftp client and server

These programs provide client-server ftp services, including:

* Server directory listing using the `-l` command.
* Upload of files from the server working directory to the client working directory using the `-g` command.
	* ASCII text and binary file upload is supported.

## Initialization

###  `ftserver` Compilation

In the directory containing ``ftserver`` files, type `make`. This will run a simple makefile that compiles ``ftserver`` into an executable.

### `ftclient` Initialization

To give ``ftclient`` executable permissions, type `chmod +x ftclient`.

## Server Execution

In the ``ftserver`` working directory, execute ``ftserver`` by typing:

`ftserver port`

* ``port`` is an integer from 1 to 65535 inclusive representing the server listening port.
* ``ftserver`` will exit with an error message if an invalid port number is specifed.

On successful execution, chatserver will give the following message:

`Server open on <PORT>
----------------------
Waiting on inbound connection...`

## Client Execution

### Execution of directory listing in `ftclient`

In the ``ftclient`` working directory, type: 

`ftclient hostname port -l data_port`

* ``hostname`` is the hostname of the machine running ``ftserver``.
* ``port`` is the ``ftserver`` listening port.
* ``-l`` is the listing command.
* ``data_port`` is the port on the ``ftclient`` machine on which the data connection will be established.

If an error occurred in validating the command line arguments, no data will be transmitted across the control connection and an error message will be displayed on the client terminal. Otherwise, if the ``ftserver`` working directory contains regular files, their names will be displayed in the ``ftclient`` window. If there are no regular files, ``ftserver`` will send an error message that ``ftclient`` will display.

### Execution of file retrieval in `ftclient`

In the ``ftclient`` working directory, type:

`ftclient hostname port -g file_name data_port`

* ``hostname`` is the same as above.
* `` port`` is the same as  above.
* ``-g`` is the file get command.
* ``file_name`` is the name of the file to be retrieved. It cannot contain forward slashes or null characters.
* ``data_port`` is the same as above.
 
If an error occurred in validation, an error message will be displayed without data transmission to ``ftserver``. If ``file_name`` matches a file in the current ``ftclient`` directory, ``ftclient`` will prompt the user to determine whether or not they want to overwrite the existing file. If the user inputs ``n``, ``ftclient`` will exit. If the user inputs ``y``, ``ftclient`` will attempt to retrieve the file from the ``ftserver`` directory. If ``ftserver`` is able to retrieve the file, a message indicating success will be displayed. If the file  could not be found, ``ftserver`` will send and error message that will be displayed by ``ftclient``.

## Cleaning up

* ``ftserver`` can be exited by pressing ``Ctrl-C`` in the server terminal window.
* The ``ftserver`` executable can be removed by typing `make clean` in the server terminal window. 
