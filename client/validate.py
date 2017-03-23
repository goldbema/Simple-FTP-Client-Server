"""
     Filename: validate.py
       Author: Maxwell Goldberg
Last Modified: 03.11.17
  Description: Provides validation utility functions.
"""

import os
import sys

MIN_OPTIONS = 5       # Minimum number of command line arguments.
PORT_MAX = 65535      # Maximum port number.
PORT_MIN = 1          # Minimum port number.

#        Method: validateMode()
#   Description: Validates the command line mode argument.
#    Parameters: inStr - The mode string.
# Preconditions: None.
#       Returns: The formatted string or -1 if the string is invalid.
def validateMode(inStr):
	if (inStr) == '-l':
		mode = 'l'
	elif (inStr) == '-g':
		mode = 'g'
	else:
		mode = -1
	return mode

#        Method: validateLen()
#   Description: Validates the command line arguments length.
#    Parameters: args - The command line arguments.
# Preconditions: None.
#       Returns: Returns True if valid, False otherwise.
def validateLen(args):
	if args[3] == '-g':
		return len(args) == MIN_OPTIONS + 1
	elif args[3] == '-l':
		return len(args) == MIN_OPTIONS	

#        Method: validatePort()
#   Description: Validates the command line port argument.
#    Parameters: port - The port argument.
# Preconditions: None.
#       Returns: Returns True if valid, False otherwise.
def validatePort(port):
	port = int(port)
	if port < PORT_MIN or port > PORT_MAX:
		return False

	return True

#        Method: validateFileName()
#   Description: Validates the file name argument.
#    Parameters: fname - The file name argument.
# Preconditions: None.
#       Returns: Returns True if valid, False otherwise.
def validateFileName(fname):
        # File name cannot contain forward slashes or null chars.
	if fname.find('/') == -1 and fname.find('\x00') == -1:
		return True
	return False

#        Method: validateFileExistence()
#   Description: Determines whether a file already exists in the directory.
#    Parameters: fname - The file name argument.
# Preconditions: None.
#       Returns: True if the file does not exist (or the user wants to overwrite
#                the file), False otherwise.
def validateFileExistence(fname):
	usrInput = ''
	
	# If the file already exists, prompt the user. 
	if os.path.isfile('./' + fname):
		print("Warning: A file with this name already exists.")
		sys.stdout.write("Would you like to overwrite it? (y/n): ")
		sys.stdout.flush()
		while True:
			usrInput = raw_input().strip()	
			if len(usrInput) > 0 and (usrInput[0] == "y" or
				usrInput[0] == "n"):
				break
			sys.stdout.write("Please enter a valid command: ")	
			sys.stdout.flush()
		# If the user opts to overwrite, return True. False otherwise.	
		if usrInput[0] == "y":
			return True
		else:
			return False
	return True
	
