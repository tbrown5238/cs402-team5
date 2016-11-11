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


//=========================================
//==  Appliance

class appliance{
public:
	//-constructors
	appliance(string type);
	~appliance(){ if(infile.is_open()){ infile.close(); } }
	
	appliance(double init_LOAD) { initialize(init_LOAD); }   //-obsolete
	void initialize(double init_LOAD);   //-obsolete

	//-input files
	int add_datafile(string fname);
	int open_file(string filename);
	int open_file(int f_index);
	int random_file();
	
	//-read files
	int next_line();
	int read_all();

	
	//-dump information (for testing)
	int dump_data(){
		/*
		print energy of each time_entry in history
		*/
		int i;
		for (i=0;i<history.size();i++){ cout << history[i].energy << endl; }
	}

	int dump_nonempty(){
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
	
	double avg_L;
	int avg_N;
	
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
