"""
     Filename: UserCommand.py
       Author: Maxwell Goldberg
Last Modified: 03.11.17
  Description: A class containing validation, packing, and unpacking utilities.
"""

import struct
import sys

import validate

class UserCommand:

        #        Method: __init__()
        #   Description: UserCommand class constructor.
        #    Parameters: None.
        # Preconditions: None.
        #       Returns: None.
	def __init__(self):
		return

	#        Method: validate()
        #   Description: Performs all validation operations on the command line.
        #    Parameters: None.
        # Preconditions: None.
        #       Returns: None. Sets class attributes.
	def validate(self):
		#If there are too few arguments, exit with error.
		if len(sys.argv) < validate.MIN_OPTIONS:
			print('ftclient: invalid number of args')
			sys.exit(1)
	
		self.sHost = sys.argv[1]
		# Validate the ftclient mode.	
		self.mode = validate.validateMode(sys.argv[3])
		if self.mode == -1:	
			print("ftclient: command must be '-g' or '-l'")
			sys.exit(1)
		# Validate the arguments length.
		if not validate.validateLen(sys.argv):
			print('ftclient: invalid number of args')
			sys.exit(1)
		# Validate the server connection port.
		if not validate.validatePort(sys.argv[2]):
			print('ftclient: invalid server port')
			sys.exit(1)
		self.sPort = int(sys.argv[2])
		# Validate the data port.
		if not validate.validatePort(sys.argv[-1]):
			print('ftclient: invalid data port')
			sys.exit(1)
		self.dPort = int(sys.argv[-1])
		# Ensure that server and data ports are different.
		if self.sPort == self.dPort:
			print('ftclient: server port and data port must be ' \
                              + 'distinct')
			sys.exit(1)	
		# If the command is a 'get file' command, validate the file
		# name and prior existence in the directory.
		if self.mode == 'g':
			if not validate.validateFileName(sys.argv[4]):
				print('ftclient: invalid filename')
				sys.exit(1)
			if not validate.validateFileExistence(sys.argv[4]):
				print('ftclient: exiting ftclient')
				sys.exit(0)
			self.fName = sys.argv[4]
		else:
			self.fName = ''

	#        Method: pack()
        #   Description: Packs the user command into a byte array.
        #    Parameters: None.
        # Preconditions: validate() has been called prior to this function.
        #       Returns: The packed byte array.
	def pack(self):
		packed = struct.pack(">bHI", ord(self.mode), self.dPort, 
                                      len(self.fName))
	        packed = bytearray(packed) + bytearray(self.fName, 'ascii')
		return packed

	#        Method: unpack()
        #   Description: Unpacks the packed byte array.
        #    Parameters: packed - The byte array to be deconstructed.
        # Preconditions: None.
        #       Returns: The unpacked tuple.
	def unpack(self, packed):
		unpacked = struct.unpack(">bhI", packed)
		return unpacked
