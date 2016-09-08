#! /usr/bin/env python
#-----------------------------------------
#	Appliance.py
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
#	temporary placeholder
#	...currently seems to be almost an exact copy of
#	one of the threaded classes from my previous project.
#	
#-----------------------------------------
import socket
import sys
import random
import re
import time
from threading import Thread


'''
Appliance.py
-- temporary placeholder

...currently seems to be almost an exact copy of
one of the threaded classes from my previous project.


This script will run independently from the System_Sim.py program.
This program acts as an appliance within the system.

'''


#=========================================
#====== Global Variables (settings) ======

RUN_SIM = True
SIM_LENGTH = 1  # number of mintues simulation will run before ending
TIMEOUT = 65.0

#--Network connection
# host = "0.0.0.0"
host = "localhost"
port = 10000
CONNECTION = socket.socket()


#------------ end of global variables ---------------------


if len(sys.argv) == 1:
	arg1 = "thermostat"
else:
	arg1 = sys.argv[1].lower()


if len(sys.argv) < 3:
	arg2 = 0
else:
	arg2 = int(sys.argv[2])




arg_lookup = {
	"thermostat":"THERM",
	"therm":"THERM",
	"heater":"PUMP",
	"pump":"PUMP",
	"heatpump":"PUMP"}

device = arg_lookup[arg1]


#=========================================
#====== System Variables (info) ==========

appl_database = {}

class Device:
	def __init__(self):
		'''default variables'''
		self.TYPE = "generic appliance"
		self.operating_levels = {"on":"1000watts", "off":"0watts"}
		self.current = "on"
		self.ID = "dev00"  #-should be unique for each appliance ... should probably just use in Simulator
		
	def usage(self):
		return self.operating_levels[self.current]
		
	def info_str(self):
		return(self.TYPE + "; " + self.ID)
#------------ end of class definition ---------------------

Appliance = Device()

if device == "THERM":
	Appliance.TYPE = "Thermostat"
	Appliance.operating_levels = {"on":"3500watts", "off":"0watts"}
	Appliance.current = "on"


elif device == "PUMP":
	Appliance.TYPE = "Waster Heater"
	Appliance.operating_levels = {"on":"3800watts", "off":"0watts"}
	Appliance.current = "on"


else:
	print("--ERROR: NO DEVICE TYPE, or not found--")
	sys.exit()

Appliance.ID = "dev{:02}".format(arg2)

print("initialized device : " + Appliance.ID)


'''
DEV_THERM = Appliance()
DEV_PUMP  = Appliance()
DEV_PUMP.TYPE = "Water Heater"
self.operating_levels = {"on":"3800watts", "off":"0watts"}
self.current = "on"
self.usage = "1000watts"

appl_database["THERM"] = DEV_THERM
appl_database["PUMP"] = DEV_PUMP

usage = "3500watts"
'''


#=========================================
#============    Functions    ============

def recieveMessage():
	'''
	Wrapper for socket.recv() for internal use (within class)
	'''
	message = ""
	try:
		message = CONNECTION.recv(512).decode()
		if not message:
			# self.is_connected = False
			print("--empty recv--")
			
	except Exception as ex:
		# self.is_connected = False
		CONNECTION.close()
		template = "An exception of type [{0}] occured.\nArguments:\n  {1!r}"
		ex_msg = template.format(type(ex).__name__, ex.args)
		print("--><--"*7)
		print(ex_msg)
		print("=-><-="*7)
		message = "---EXCEPTION---"
	message = message.rstrip("\n")
	return message


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


#------------ end of function definition ------------------


#=========================================
#===========    begin MAIN    ============

#-----------------------------------------
#-  Connect to Simulator
CONNECTION.connect((host, port))
CONNECTION.settimeout(TIMEOUT)

#-send Appliance info
incoming = recieveMessage()
if incoming == "getInfo":
	my_send(CONNECTION, Appliance.info_str())
else:
	print("error, expected 'getInfo', but got [{}] instead".format(incoming))
	sys.exit()

apple = "apple"


print("--------------------------------------------------")


#-----------------------------------------
#-  Run simulation

minutes = 0
while RUN_SIM:
	#-send current data to simulator
	current_data = Appliance.usage() #-redundant
	incoming = recieveMessage()
	
	if incoming == "getData":
		my_send(CONNECTION, Appliance.usage())
	else:
		print("error, expected 'getData', but got [{}] instead".format(incoming))
		sys.exit()
		
	minutes += 1
	print(" - sent data #{}".format(minutes))

	#--> should modify RUN_SIM somewhere to prevent infinite loop...
	if minutes > SIM_LENGTH : break
	time.sleep(5)

print("--simulation complete--")

time.sleep(5)
any_key = input("enter any key to exit")

# input_str = "empty"

# while input_str is "empty":
	# input_str = input("enter any key to exit")



print("you chose the [{}] 'key'".format(any_key))

## end program
