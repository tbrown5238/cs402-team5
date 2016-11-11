/* ==========================================
*	FILE:	 run_test.cpp
*	PROJECT: Making Appliances Self Aware
*	AUTHOR:	 Travis Brown, Jason Derrick, Mark Bolles, Ryan Tungett
*	DATE:	 2016-10-18
*
*	* This program manipulates the device and appliance objects to simulate
*	  energy usage and communicate with other instances of this program.
*
========================================== */

// #include <arpa/inet.h>
// #include <errno.h>
#include <netdb.h>
#include <stdlib.h>
// #include <sys/types.h>
// #include <sys/socket.h>
#include <unistd.h>

#include <string.h>
#include <string>
#include <vector>

#include <iostream>
#include <fstream>
#include <sstream>

#include "time_entry.h"
#include "appliance.h"
#include "bidding.h"

#include "comm.h"


using namespace std;


//=========================================
//==  Global Variables (settings)


//--choose 1
// string input_file   = fname_CC_01;


//--other options, variables
string DELIM = "------------------------------------------";

//------------ end of global variables ---------------------


//=========================================
//==  program info, etc

void print_usage(){  cout << "Usage: ./run_test [type] [id]" << endl;  }

//------------ end of program info etc ---------------------



//=========================================
//==  Functions|Prototypes

int checkpoint(int timeA, int timeB) {
  return timeA - timeB;
}

//------------ end of function definition ------------------


//=========================================
//==  begin "MAIN"
int main(int argc, char* argv[]){
	
	srand(time(NULL));
	//-----------------------------------------
	//-  Initialize, parse command line
	
	if(argc < 3){
		print_usage();
		return(-1);
	}
	string appl_type = argv[1];
	string myID = "0" + (string)argv[2];
	string my_info;  //-string to send to sim_server to store as metadata
	string input_file = "tmp_string";
	double setLOAD;
	string Device_arg;
	
	if(appl_type == "CC"){
		my_info = "CarCharger; dev" + myID;
		Device_arg = "carcharger";
		// setLOAD = 6.6;
	}else if(appl_type == "HV"){
		my_info = "HVAC; dev" + myID;
		Device_arg = "hvac";
		// setLOAD = 0.25;
	}else if(appl_type == "PP"){
		my_info = "PoolPump; dev" + myID;
		Device_arg = "poolpump";
		// setLOAD = 0.4;
	}else if(appl_type == "WH"){
		my_info = "WaterHeater; dev" + myID;
		Device_arg = "waterheater";
		// setLOAD = 3.0;
	}else{
		cerr << "Error, appliance type [" << appl_type << "] not recognized" << endl;
		my_info = "CarCharger; dev" + myID;
		Device_arg = "carcharger";
	}
	
	cout << endl;
	
	// appliance ME(setLOAD);
	appliance ME(appl_type);
	
	
	//-----------------------------------------
	//-  integrate Device class
	
	cout << "----creating device----------------------" << endl;
	Device device(Device_arg.c_str());
	cout << device.power << "  " << device.failed_bids << endl;
	cout << "----^^ should NOT be an empty line ^^----" << endl;
	
	
	//-----------------------------------------
	//-  connect to simulation server

	comm COMM;
	if(COMM.c_connect()){
		cout << "-=-=-=-=--=-=-=-=-| Connection established |-=-=--=-=-" << endl;
	} else{
		cout << "-!!!-!!!-| FAILED CONNECTION |-!!!-" << endl;
		exit(-1);
	}
	
	//-----------------------------------------
	//-  generate list of input files
	
	// ME.open_file(input_file);
	
	// ME.add_datafile(input_file);
	
	// ME.add_datafile(input_file2);
	// ME.add_datafile(input_file3);
	// ME.add_datafile(input_file4);
	
	ME.random_file();
	
	
	
	//-----------------------------------------
	//-  Read input, dump output
	
	//-recieve initial message ("getData")
	string response;
	response = COMM.c_recv();

	//-reply with device data
	COMM.c_send(my_info);


	string send_line;
	string recv_line;
	
	stringstream ss;
	int N = 0;
	int connected_devices = 1;
	//-simulate passing of time;
	//---each time it calls next_line() another 'minute' has passed
	while(ME.next_line()){
		N++;
		
		//-while testing, pring checkpoint every 120m
		if(N%120 == 0){
			
			cout << "------ - - | " << N << " | - - - - -  - - - - - - -------------------------" << endl;
			//-construct string from data and send to server
			ss.clear ();
			ss.str ("");
			// ss << myID << "  " << N << "  balance:  " << ME.Balance;
			ss << N << ";" << myID << ";" << ME.Balance;
			send_line = ss.str();
			COMM.c_send(send_line);
			//-reset balance (testing)
			ME.Balance = 0;
			
			//-recieve acknowledgement
			recv_line = COMM.c_recv();
			
			//-get number of connected devices
			connected_devices = stoi(COMM.c_recv());
			
			// cout << " <<-- [" << send_line << "]" << endl;
			// cout << "-->>  [" << recv_line << "]" << endl;
			cout << " # connected: " << connected_devices << endl;
			
			cout << "\\______________________________________________________________________/" << endl << endl;
		}
		
		if(N>1450){ break; }  //-prevent infinite loop, just in case
	}
	
	send_line = "exit";
	COMM.c_send(send_line);
	
	
	//-----------------------------------------
	//-  bookkeeping

	//-Close socket
	COMM.c_close();
	
	
	cerr << "happy exit" << endl << "hit enter to exit" << endl;
	cin.ignore();
	
	// return(1);
	return(0);
} // end MAIN

