#! /usr/bin/env python
#-----------------------------------------
#	System_Sim.py
#	
#	cs402: Making Appliances "Self Aware"
#	
#	Jason Derrick (lead)
#	Mark Bolles
#	Travis Brown
#	Ryan Tungett
#-----------------------------------------
#	
#------------
#	
#-----------------------------------------
import socket
import sys
import random
import re
import time
from threading import Thread


#=========================================
#====== Global Variables (settings) ======

RUN_SIM = True


#--Network connection
host = "0.0.0.0"
port = 10000
LISTENER = socket.socket()
LISTENER.bind((host, port))

#--System Info
Devices = []      # list of connected appliances
N_Appliances = 2  # target number of appliances (hardcoded for simplicity right now)
history = {}      # historical data, indexed on timestamp

#------------ end of global variables ---------------------


#=========================================
#=============    Classes    =============


class Device():
	def __init__(self, connection):
		self.connection = connection
		self.is_connected = True
		self.type = "undetermined"
		self.ID = "dev00"
		
	def recieveMessage(self):
		message = ""
		try:
			message = self.connection.recv(512).decode()
			if not message:
				self.is_connected = False
				print("--empty recv--")
				
		except Exception as ex:
			self.is_connected = False
			self.connection.close()
			template = "An exception of type [{0}] occured.\nArguments:\n  {1!r}"
			ex_msg = template.format(type(ex).__name__, ex.args)
			print("--><--"*7)
			print(ex_msg)
			print("=-><-="*7)
			message = "---EXCEPTION---"
		message = message.rstrip("\n")
		return message
		
	def getInfo(self):
		M = "getInfo"
		self.connection.send(M.encode())  #socket_send
		infostring = self.recieveMessage()
		info = infostring.split(str=";")
		self.type = info[0]
		self.ID = info[1]
		self.type = self.type.strip()
		self.ID = self.ID.strip()
		return
		
	def getData(self):
		M = "getData"
		self.connection.send(M.encode())  #socket_send
		datastring = self.recieveMessage()
		#--> should probably include some error checking here
		return datastring.strip()
#------------ end of Device() class -----------------------


class Snapshot():
	def __init__(self, timestamp):
		self.time = timestamp
		self.data = {}  # list/dict of recorded info at given timestamp
		
	def add_data(self, Did, r):
		self.data[Did] = r  # Did: Device ID, r: record (current state/power usage)
		return
		
#------------ end of class definition ---------------------


#=========================================
#============    Functions    ============

def waitConnection():
	'''
	listen for a client to connect, then accept connection
	Return connected socket with a 60 sec timeout
	return False if Exception occured
	'''
	try:
		print("..listening..", end=" ")
		LISTENER.listen(5)
		print(" .. ..")
		c, addr = LISTENER.accept()
		print("__accepted__")
	except Exception as ex:
		template = "An exception of type [{0}] occured.\nArguments:\n  {1!r}"
		ex_msg = template.format(type(ex).__name__, ex.args)
		print("--><--"*7)
		print(ex_msg)
		print("=-><-="*7)
		return False
	c.settimeout(60.0)
	print("timeout set to 60")
	return c


def my_send(sock, M):
	'''
	wrapper for socket's send function
	
	Currently just prints message before appending EOL
	Can be expanded to check syntax, etc
	'''
	print("<<< [{}]".format(M))
	
	if M == "":
		print("--< (sending empty message...)")
		M += "\n"
	elif M[-1] == "\n":
		print("--< already appended")
	else:
		M += "\n"
	sock.send(M.encode())  #socket_send
	return


def tunnel(A, msg):
	'''recieve msg from A, save info to database, send msg to all other connections'''
	return
#------------ end of function definition ------------------


#=========================================
#===========    begin MAIN    ============

#-----------------------------------------
#-  Connect to Appliances
print("\n--------------------------------------------------\n")
while Devices.size() < N_Appliances:
	#-connect via waitConnection()
	conn_sock = waitConnection()
	if not conn_sock:
		print("--no connection--")
		#-device never connected
		continue
		
	newDevice = Device(conn_sock)
	newDevice.getInfo()
	#--> check Device.ID for uniqueness
	
	
	for G in game_list:
		msg = "game: "
		msg += G.getGameInfo()
		my_send(conn_sock, msg)
	print(".:.:. conn_sock was")
	#--> send list of currently connected appliances
	#--> send new connection info to currently connected appliances
		
print("--------------------------------------------------")


#-----------------------------------------
#-  Run simulation

while RUN_SIM:
	T = time.now()  #--?SYNTAX?--#
	tmp_S = Snapshot(T)
	#-retrieve data from Devices
	for D in Devices:
		tmp_S.add_data(D.ID, D.getData())
		#--> send data to each other Device
	#-save data
	history[T] = tmp_S
	#--> should modify RUN_SIM somewhere to prevent infinite loop...
	time.wait(60)   #--?SYNTAX?--#

#=========================================

## end program



#-----------------------------------------
#-  ToDo
'''

add print() function for history{} ... and somewhere in the program to print/record the data


notes:
- the record/snapshot/whatever data is being stored could use some consistency, if not some improvement.


'''


