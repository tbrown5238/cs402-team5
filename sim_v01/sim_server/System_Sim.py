#! /usr/bin/python
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
#	Python server to collect data from individual devices and create output display.
#------------
#	
#-----------------------------------------
from __future__ import print_function

import socket
import sys
import random
import re
import time
from threading import Thread


#=========================================
#====== Global Variables (settings) ======

RUN_SIM_SIMPLE = True
RUN_SIM = False
SIM_LENGTH = 14  # number of "mintues" simulation will run before ending


#--Network connection
# host = "0.0.0.0"
host = "localhost"
port = 10000
LISTENER = socket.socket()
LISTENER.bind((host, port))
TIMEOUT = 65.0

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
		self.getInfo()
		
	def recieveMessage(self):
		'''
		Wrapper for socket.recv() for internal use (within class)
		'''
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
		'''
		Run during constructor to retrieve device information
		'''
		M = "getInfo"
		print(">> getInfo:send")
		self.connection.send(M.encode())  #socket_send
		print("<< getInfo:recieveMessage")
		infostring = self.recieveMessage()
		# info = infostring.split(str=";")
		info = infostring.split(";")
		self.type = info[0]
		self.ID = info[1]
		self.type = self.type.strip()
		self.ID = self.ID.strip()
		return
		
	def getData(self):
		'''
		recieve message from Device,
		return message as a string
		'''
		M = "getData"
		print(">> getData:send")
		self.connection.send(M.encode())  #socket_send
		print("<< getData:recieveMessage")
		datastring = self.recieveMessage()
		#--> should probably include some error checking here
		return datastring.strip()
		
	def getLine(self):
		'''
		recieve message from Device,
		return message as a string
		--WITHOUT sending message first--
		'''
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
		
	def __str__(self):
		'''format data as a string'''
		R = str(self.time)
		for devID, rec in self.data.items():
			R += "  -  {}:{}".format(devID, rec)
		return R
	
		
#------------ end of class definition ---------------------


#=========================================
#============    Functions    ============

def waitConnection():
	'''
	listen for a client to connect, then accept connection
	Return connected socket with a 60 sec timeout
	return False if Exception occured
	'''
	# TIMEOUT = 65.0
	try:
		print("..listening..", end=" ")
		LISTENER.settimeout(TIMEOUT)
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
	c.settimeout(TIMEOUT)
	print("timeout set to {:02}s".format(TIMEOUT))
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
backup = 0
while len(Devices) < N_Appliances:
	backup += 1
	if backup > (N_Appliances*2): break
	#-connect via waitConnection()
	conn_sock = waitConnection()
	if not conn_sock:
		print("--no connection--")
		#-device never connected
		continue
		
	newDevice = Device(conn_sock)
	# newDevice.getInfo()
	#--> check Device.ID for uniqueness
	
	Devices.append(newDevice)
	
	#--> send list of currently connected appliances
	#--> send new connection info to currently connected appliances
	#-->  ... actually, just let Device connect to Sim, and let Sim handle all communications

print("--------------------------------------------------")


#-----------------------------------------
#-  Run simulation

minutes = 0
N = 0
break_flag = False
while RUN_SIM_SIMPLE:
	N += 1
	#-retrieve data from Devices
	for D in Devices:
		tmpLine = D.getLine()
		history[N] = tmpLine
		my_send(D.connection, "--acknowledged--")
		
		if tmpLine.lower() == "exit" : break_flag = True
		# print("[" + tmpLine + "]")
	minutes += 1
	print("saved data #{}".format(minutes))
	#--> should modify RUN_SIM somewhere to prevent infinite loop...
	if break_flag : break
	if minutes > SIM_LENGTH : break
	time.sleep(1)

minutes = 0
N = 0
while RUN_SIM:
	N += 1
	# T = time.time()
	T = N
	tmp_S = Snapshot(T)
	#-retrieve data from Devices
	for D in Devices:
		tmp_S.add_data(D.ID, D.getData())
		#--> send data to each other Device
	#-save data
	history[T] = tmp_S
	minutes += 1
	print("saved data #{}".format(minutes))
	
	#--> should modify RUN_SIM somewhere to prevent infinite loop...
	if minutes > SIM_LENGTH : break
	# time.sleep(6)

#=========================================


#-----------------------------------------
#-  Print results

print("--------------------------------------------------")
for S in history:
	# print(history[S])
	print("[" + history[S] + "]")
	
print("--------------------------------------------------")


time.sleep(2)
# any_key = input("enter any key to exit")  #-python3
any_key = raw_input("enter any key to exit\n")  #-python2

print("you chose the [{}] 'key'".format(any_key))

## end program



#-----------------------------------------
#-  ToDo
'''

add print() function for history{} ... and somewhere in the program to print/record the data


notes:
- the record/snapshot/whatever data is being stored could use some consistency, if not some improvement.


'''

