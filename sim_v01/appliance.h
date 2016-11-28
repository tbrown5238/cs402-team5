#ifndef APPLIANCE_H
#define APPLIANCE_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;


//=========================================
//==  Global Variables (settings)

// string DELIM = "------------------------------------------";
// string DELIM;
extern string DELIM;

enum a_state { ON, OFF, STANDBY, OVERRIDE };
// ON  - appliance has consumed power in the past minute
// OFF  - appliance has NOT consumed power in the past minute
// STANDBY  - device has shut off appliance, aka "shed"
// OVERRIDE  - manual override; customer has forced "on"

//=========================================
//==  Appliance

class Appliance{
public:
	//-constructors
	Appliance(string type);
	~Appliance(){ if(infile.is_open()){ infile.close(); } }
	
	//-input files
	int open_file(string filename);
	int open_file(int f_index);
	int random_file();
	
	//-read files
	int next_line();
	int read_all();  //-was used for testing
	
	//-operation/communication
	bool is_override(){    //-check for override
		if(current_state == OVERRIDE){ return(true); }
		else{ return(false); }
	}
	bool enter_standby(); //-attempt to enter standby state; reterns true on success
	bool exit_standby();  //-lifts "standby" mode, re-enters "on" state

	void update_state();  //-updates current_state based on Balance and current_state
						  //-check state and determine if the appliance wants to spend energy
	
	double spent();       //-determine how much energy WAS spent in the last minute
	
	void to_spend();      //-determine how much energy WILL BE spent
	// ^v duplicate?
	double spend_energy();//-determine how much energy was consumed in the previous minute

	
	//-dump information (for testing)
	void dump_data(){
		/*
		print energy of each time_entry in history
		*/
		int i;
		for (i=0;i<history.size();i++){ cout << history[i].energy << endl; }
	}

	void dump_nonempty(){
		/*
		print all lines with a non-zero load
		--also calculates LOAD based on average of these values
		---currently using hard-coded (assigned) values for LOAD
		*/
		int i;
		for (i=0;i<history.size();i++){
			if (history[i].energy > 0){
				cout << history[i].energy << endl;
				avg_L += history[i].energy;
				avg_N++;
			}
		}
		// LOAD = avg_L/avg_N;
	}
	
// protected:
	ifstream infile;
	double LOAD;    //-power consumption of the appliance
	double Balance; //-running total of energy that needs to be spent
	double spend;   //-amount of energy to be consumed in the upcoming minute
	bool needs_to_run; //-used to check if appliance needs to run
	
	double avg_L;
	int avg_N;
	
	string ID;
	int minutes_standby;
	
	int prev_time;
	a_state current_state;
	
	string current_fname;
	vector <string> datafiles;
	
	vector <time_entry> history;
	
	//--input files
	vector <string> CC_filelist;
	vector <string> HV_filelist;
	vector <string> PP_filelist;
	vector <string> WH_filelist;
	int num_CC_files;
	int num_HV_files;
	int num_PP_files;
	int num_WH_files;

};


#endif
