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
#include <list>

#include <iostream>
#include <fstream>
#include <sstream>

#include "time_entry.h"
#include "appliance.h"
// #include "bidding.h"
#include "q_learning.h"

#include "comm.h"


using namespace std;


//=========================================
//==  Global Variables (settings)


//--other options, variables
string DELIM = "------------------------------------------";

//------------ end of global variables ---------------------


//=========================================
//==  program info, etc

void print_usage(){  cout << "Usage: ./run_test [type] [id]\n\t--> [id] no longer used" << endl;  }

//------------ end of program info etc ---------------------



//=========================================
//==  History class

class History{
//-wrapper for History list
public:
	History(){ energy_history.resize(15); }
	~History(){}
	
	//-add new energy value to the list
	void update(double val){
		//pop off front
		energy_history.pop_front();
		//push [val] to back
		energy_history.push_back(val);
		return;
	}
	
	//-get average of past 15min
	double average(){
		double avg = 0.0;
		
		// for(int i=0;i<15;i++){
			// avg += energy_history[i];
		// }
		
		// for(list<double>::iterator it = energy_history.begin(); it != energy_history.end(); ++it){
		for(auto it = energy_history.begin(); it != energy_history.end(); ++it){
		// for(double V : energy_history){
			avg += *it;
			// avg += V;
		}
		
		avg = avg/15;
		return(avg);
	}
	list<double> energy_history;
	
};
//------------ end of History class ------------------------



//=========================================
//==  Functions|Prototypes

int checkpoint(int timeA, int timeB) {
  return timeA - timeB;
}

//------------ end of function definition ------------------


//=========================================
//==  begin "MAIN"
int main(int argc, char* argv[]){
	int MAX_DAYS = 30;
	int SIM_LENGTH = 1440*MAX_DAYS;
	srand(time(NULL));
	
	double TIER = 4.5; //-try to keep avg power consumption under this
	
	//-----------------------------------------
	//-  Initialize, parse command line
	
	if(argc < 2){
		print_usage();
		return(-1);
	}
	string appl_type = argv[1];
	// string myID = "0" + (string)argv[2];
	string myID = "00";  //-no longer from commandline, assigned by server
	string my_info;  //-string to send to sim_server to store as metadata
	
	string Device_arg; //-for initializeing Device class (bidding.h); not currently used
	
	//--? what were these for?
	// string input_file = "tmp_string";
	// double setLOAD;
	
	if(appl_type == "CC"){
		// my_info = "CarCharger; dev" + myID;  
		my_info = "CarCharger;";
		Device_arg = "carcharger";
	}else if(appl_type == "HV"){
		// my_info = "HVAC; dev" + myID;
		my_info = "HVAC;";
		Device_arg = "hvac";
	}else if(appl_type == "PP"){
		// my_info = "PoolPump; dev" + myID;
		my_info = "PoolPump;";
		Device_arg = "poolpump";
	}else if(appl_type == "WH"){
		// my_info = "WaterHeater; dev" + myID;
		my_info = "WaterHeater;";
		Device_arg = "waterheater";
	}else{
		cerr << "Error, appliance type [" << appl_type << "] not recognized" << endl;
		my_info = "CarCharger; dev" + myID;
		Device_arg = "carcharger";
	}
	cout << endl;
	
	Appliance ME(appl_type);
	
	
	//-----------------------------------------
	//-  integrate Device class
	
	// cout << "----creating device-----------------------" << endl;
	// Device device(Device_arg.c_str());
	// cout << device.power << "  " << device.failed_bids << endl;
	// cout << "----^^ should NOT be an empty line ^^-----" << endl;
	
	Q_learning Q;
	
	//-----------------------------------------
	//-  connect to simulation server
	
	comm COMM;
	if(COMM.c_connect()){
		cout << "-=-=-=-=--=-=-=-=-| Connection established |-=-=--=-=-" << endl;
	} else{
		cout << "-!!!-!!!-| FAILED CONNECTION |-!!!-" << endl;
		exit(-1);
	}
	
	//-------
	

	//-----------------------------------------
	//-  Read input, dump output
	
	//-recieve initial message ("getData")
	string response;
	response = COMM.c_recv();

	//-reply with device data
	COMM.c_send(my_info);
	
	//-receive IDnumber
	myID = COMM.c_recv();
	cout << "I am number : " << myID << endl;
	ME.ID = myID;

	
	//-----------------------------------------
	//-  select an input file
	ME.random_file();


	string send_line;
	string recv_line;
	
	stringstream ss;
	int i = 0;  // counter for loops
	int N = 0;  // total "minutes" passed
	int N_min = 0; // time of day
	int N_day = 1; // day number
	int connected_devices = 1;
	double energy_spent_total = 0.0;
	double energy_spent_dev = 0.0;
	double energy_spent = 0.0;
	double diff = 0.0;  // difference between spent and TIER
	History H;
	int decision = 0;
	
	//-simulate passing of time;
	//---each time it calls next_line() another 'minute' has passed
	cerr << "beginning read" << endl;
	while(ME.next_line()){
		N++;
		// cerr << "--<"+(string)myID+">---- - - | " << N << " | - - - - -  - - - - - - -------------------------" << endl;
		N_min = N%1440;
		
		
		//-while testing, print checkpoint every 120m|3m
		if(N%1 == 0){
			
			//-construct string from data and send to server
			ss.clear();
			ss.str("");
			ss << N << ":" << myID << ";" << ME.spent();
			send_line = ss.str();
			COMM.c_send(send_line);
			
			//-recieve acknowledgement
			recv_line = COMM.c_recv();
			
			if(recv_line == "--EXIT--"){
				cerr << "--!!--EXIT--!!--" << endl;
				break;
			}
			
			//-get number of connected devices
			connected_devices = stoi(COMM.c_recv());
			COMM.c_send("acknowledged");
			
			
			//-reset energy
			energy_spent_total = 0.0;
			//-get current power usage from each other device
			for(i=0;i<connected_devices;i++){
				recv_line = COMM.c_recv();
				COMM.c_send("acknowledged");
				
				//-parse data, update total power usage
				//==========================
				// /*
				size_t T = recv_line.find(';', 3);
				if (T != std::string::npos){
					recv_line = recv_line.substr (T+1, recv_line.length()-1);
				}
				else{
					cerr << " ~~can't find ;(energy_usage)~~ [" << recv_line << "]" << endl;
				}
				cout << "atof(" << recv_line << ")" << endl;
				
				
				energy_spent_dev = atof(recv_line.c_str());
				// */
				//--------------------------
				/*
				char search[] = recv_line.c_str();
				char * tok;
				tok = strtok(search,";");
				tok = strtok(NULL,";");
				
				cerr << "--(tok: " << tok << ")--" << endl;
				// */
				
				//==========================

				energy_spent_total += energy_spent_dev;
			}
			// cout << "==[" << energy_spent_total << "]==" << endl;
			H.update(energy_spent_total);
			
			
			
			//----BIDDING/MAKE DECISION
			//.. goes here ...
			
			decision = Q.get_decision(H.average(), TIER, ME.minutes_standby);
			decision %= 2;
			// cerr << "^^ " << decision << " ^^" << endl;
			diff = TIER - H.average();
			if(ME.needs_to_run){
				//-make decision: Do I remain on, or turn off?
				// decision = Q.get_decision(energy_spent_total.average(), TIER, ME.minutes_standby);
				
				// /*
				if(H.average() < (TIER*.75)){
					//-if average is less than 75% maximum, don't standby
					ME.exit_standby();
					//continue;
				}
				// */
				else if(!decision){
				// if(!decision){
					//-testing: enter standby at 7am
					if(ME.current_state != STANDBY){
						cout << "^^ " << decision << " ^^[" << N_min << "]--" << myID << "--" << endl;
						// cerr << "[[--" << myID << "--]]" << endl;
						ME.enter_standby();
					}
				}
				else if(decision){
					//-testing: exit standby at 10am
					if(ME.current_state == STANDBY){
						cout << "^^ " << decision << " ^^[" << N_min << "]++" << myID << "++" << endl;
						// cerr << "[[++" << myID << "++]]" << N << endl;
						ME.exit_standby();
					}
				}
				/*
				if((N_min >= 420) && (N_min < 600)){
					//-testing: enter standby at 7am
					if(ME.current_state != STANDBY){
						cerr << "--" << N << endl;
						ME.enter_standby();
					}
				}
				if(ME.current_state == STANDBY){
					//-testing: exit standby at 10am
					if(N_min >= 600){
						cerr << "++" << myID << "++" << N << endl;
						ME.exit_standby();
					}
				}
				// */
					
				//---(testing)--> remain on
				// ME.Balance = 0;
				
				//-update Balance
				ME.spend_energy();
			}
		}
		
		if(N_min==0){
			//-End of day
			cerr << "day#" << N_day << ",  file: " << ME.current_fname << endl;
			cout << "day#" << N_day << ",  file: " << ME.current_fname << endl;
			N_day++;
		}
		
		if(N>SIM_LENGTH){
			cerr << "--!!-- capped out at [" << SIM_LENGTH << "] --" << endl;
			break;
		}  //-prevent infinite loop, just in case
	}
	
	send_line = "exit";
	COMM.c_send(send_line);
	
	
	//-----------------------------------------
	//-  bookkeeping

	//-Close socket
	COMM.c_close();
	
	sleep(1);
	cerr << "\n happy exit :)" << endl;
	sleep(1);
	cerr << " ...";
	// for(i=0;i<4;i++){
		// sleep(1);
		// cerr << " ...";
	// }
	cerr << endl;
	sleep(1);
	return(0);
} // end MAIN

