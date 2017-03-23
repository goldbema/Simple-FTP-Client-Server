"""
     Filename: ClientSocket.py
       Author: Maxwell Goldberg
Last Modified: 03.11.17
  Description: A class that provides an interface to the socket API.
"""

import socket
import sys

from UserCommand import UserCommand

HEADER_LEN = 7

class ClientSocket:
	
	#        Method: __init__()
	#   Description: The ClientSocket class constructor.
	#    Parameters: sock - Optional socket parameter.
	# Preconditions: None.
	#       Returns: None.
	def __init__(self, sock=None):
		if sock is None:
			self.sock = socket.socket(socket.AF_INET,
				 socket.SOCK_STREAM)
		else:
			self.sock = sock

	#        Method: connect()
	#   Description: Attempts to connect to host:port.
	#    Parameters: host - The hostname
	#                port - The port name
	# Preconditions: The socket has been initialized.
	#       Returns: None.
	def connect(self, host, port):
		try:
			self.sock.connect((host, port))
		except socket.error as e:
			print('ftclient: connect:{0}'.format(e))
			sys.exit(2)

	#        Method: send()
	#   Description: Attempts to send an entire message.
	#    Parameters: msg - The message to send.
	# Preconditions: The socket has been initialized.
	#       Returns: None.
	def send(self, msg):
		totalSent = 0
		while totalSent < len(msg):
			sent = self.sock.send(msg[totalSent:])
			if sent == 0:
				raise RuntimeError("send: socket " + \
						"connection broken")
			totalSent += sent

	#        Method: receive()
	#   Description: Attempts to receive a message.
	#    Parameters: None.
	# Preconditions: The socket has been initialized.
	#       Returns: The message body.
	def receive(self):
		uc = UserCommand()
		body = ''
		# Receive the header.
		header = self._receive(HEADER_LEN)
		# Unpack the header.
		unpackedHeader = uc.unpack(header)
		bodyLen = unpackedHeader[2]
		# Receive the message body.
		if bodyLen > 0:
			body = self._receive(bodyLen)
		return body

	#        Method: _receive()
	#   Description: Receives msgLen bytes of a message.
	#    Parameters: msgLen - The length of the message.
	# Preconditions: The socket has been initialized.
	#       Returns: The message body.
	def _receive(self, msgLen):
		chunks = []
		bytesReceived = 0

		while bytesReceived < msgLen:
			chunk = self.sock.recv(msgLen - bytesReceived)
			if chunk == '':
				raise RuntimeError("ftclient: recv: socket " + \
						"connection broken")
			chunks.append(chunk)
			bytesReceived = bytesReceived + len(chunk)

		return ''.join(chunks)
