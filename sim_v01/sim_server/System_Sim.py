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

#-table to lookup power rating
device_table = {
	"CarCharger":6.6,
	"HVAC":0.25,
	"PoolPump":0.4,
	"WaterHeater":3.0}

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
	def __init__(self, connection, next_ID):
		self.connection = connection
		self.is_connected = True
		self.type = "undetermined"
		self.power_rating = 0.5
		self.ID = next_ID
		self.getInfo()
		print("--I AM:\n {}  ({}) : {}".format(self.ID, self.type, self.power_rating))
		self.last_msg = ""
		
	def getInfo(self):
		'''
		Run during constructor to retrieve device information
		'''
		M = "getInfo\n"
		print(">> getInfo:send")
		self.connection.send(M.encode())  #socket_send
		print("<< getInfo:recieveMessage")
		infostring = self.recieveMessage()
		# info = infostring.split(str=";")
		info = infostring.split(";")
		self.type = info[0]
		# self.ID = info[1]
		self.type = self.type.strip()
		self.power_rating = device_table.get(self.type, 0.5)
		# self.ID = self.ID.strip()
		
		M = "{}\n".format(self.ID)
		self.connection.send(M.encode())  #socket_send
		
		return
		
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
		self.last_msg = message
	
		print("  >> {}-[{}]".format(self.ID, message))
	
		return message
		
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
		
	def info_str(self):
		'''
		return formatted string with device information
		'''
		return "{},{}".format(self.ID, self.power_rating)
		
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
		print("..listening.. @ {}.{}".format(host, port), end=" ")
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


def my_send(sock, ID, M):
	'''
	wrapper for socket's send function
	
	Currently just prints message before appending EOL
	Can be expanded to check syntax, etc
	'''
	print(" <<  {}-[{}]".format(ID, M))
	
	if M == "":
		print("--< (sending empty message...)")
		M += "\n"
	elif M[-1] == "\n":
		print("--< already appended")
	else:
		M += "\n"
	sock.send(M.encode())  #socket_send
	return


def bid_to_device(A, msg):
	'''send a single bid to a device'''
	return


def bid_from_device(A, msg):
	'''recieve a device's bid'''
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
''' TODO:
- modify connection protocol to allow connect/disconnect of devices
-- convert Device to a threaded class

'''
next_ID = 1
while len(Devices) < N_Appliances:
	backup += 1
	if backup > (N_Appliances*2): break
	#-connect via waitConnection()
	conn_sock = waitConnection()
	if not conn_sock:
		print("--no connection--")
		#-device never connected
		continue
		
	newDevice = Device(conn_sock, next_ID)
	next_ID += 1
	# newDevice.getInfo()
	#--> check Device.ID for uniqueness
	#--> nevermind, ID is now assigned by server
	
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
	minutes += 1
	#-retrieve data from Devices
	print("|----------------------------------------------------------------------|")
	for D in Devices:
		tmpLine = D.getLine()
		history[N] = tmpLine
		my_send(D.connection, D.ID, "--acknowledged--")
		
		if tmpLine.lower() == "exit" : break_flag = True
	
	if break_flag : break
	
	print(" ----------  saved data #{}  ---------------- ".format(minutes))
	
	#-respond with number of connected devices
	for D in Devices:
		my_send(D.connection, D.ID, str(len(Devices)))
		# my_send(D.connection, D.ID, str(minutes))
	
	print("\\______________________________________________________________________/")
	
	#--> should modify RUN_SIM_SIMPLE somewhere to prevent infinite loop...
	if minutes > SIM_LENGTH : break
	time.sleep(1)

#=========================================


#-----------------------------------------
#-  Print results

print("--------------------------------------------------")
for S in history:
	print("[" + history[S] + "]")
	
print("--------------------------------------------------")


time.sleep(1)
# any_key = input("enter any key to exit")  #-python3
any_key = raw_input("enter any key to exit\n")  #-python2

# print("you chose the [{}] 'key'".format(any_key))

## end program



#-----------------------------------------
#-  ToDo
'''

add print() function for history{} ... and somewhere in the program to print/record the data


notes:
- the record/snapshot/whatever data is being stored could use some consistency, if not some improvement.


'''

