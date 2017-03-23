"""
     Filename: filemgmt.py
       Author: Maxwell Goldberg
Last Modified: 03.11.17
  Description: Provides a utility to write a string to file.
"""

#        Method: strToFile()
#   Description: Writes a string to file.
#    Parameters: fname - The filename to be written to.
#                data - The string to be written to file.
# Preconditions: fname and data are both strings.
#       Returns: None.
def strToFile(fname, data):
	fp = open(fname, "w+")
	fp.write(data)
	fp.close()
