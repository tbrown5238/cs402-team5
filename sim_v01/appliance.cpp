/* ==========================================
*	FILE:	 appliance.cpp
*	PROJECT: Making Appliances Self Aware
*	AUTHOR:	 Travis Brown, Jason Derrick, Mark Bolles, Ryan Tungett
*	DATE:	 2016-10-18
*
*	* This program acts as the appliance for testing the algorithm in our device.
*	  It determines the appliance's state (on/off/standby/override) and communicates
*	  with our device.
*
========================================== */

#include "time_entry.h"
#include "appliance.h"

using namespace std;


extern string DELIM;


//=========================================
//==  Appliance


//-----------------------------------------
//-  constructors

  /*
constructor  */
void appliance::initialize(double init_LOAD){
	LOAD = init_LOAD;
	Balance = 0;
	prev_time = 0;
	current_state = OFF;
	avg_L = 0;
	avg_N = 0;
}


//-----------------------------------------
//-  input files

  /*
add string to list of data (input) files  */
int appliance::add_datafile(string fname){
	datafiles.push_back(fname);
	return(1);
}

  /*
open file by string  */
int appliance::open_file(string filename){
	current_fname = filename;
	//-verbose:
	cout << DELIM << endl;
	cout << " opening: " << current_fname << endl;
	
	infile.open(filename.c_str());
	return(1);
}

  /*
open file by index  */
int appliance::open_file(int f_index){
	//-if negative (invalid) argument, open random file instead
	if(f_index<0){
		cerr << "-!!-Error: open_file(index) : cannot open negative index, opening random instead\n";
		int f_num;
		f_num = random_file();
		return f_num;
	}

	//-make sure index is within range
	if(f_index>=datafiles.size()){
		//--?possibly do a % until f_index is wihtin range?
		cerr << "-!!-Error: open_file(index) outside range\n";
		return(-1);
	}
	
	current_fname = datafiles[f_index];
	//-verbose:
	// cout << DELIM << endl;
	// cout << " opening: " << current_fname << endl;
	
	open_file(current_fname);
	return(1);
}

  /*
selects and opens file from list, chosen randomly  */
int appliance::random_file(){
	int max = datafiles.size();
	int i = 0;
	i = rand()%max;
	
	current_fname = datafiles[i];
	open_file(current_fname);
	// read_all();  //-for testing
	
	
	//---uncomment below to print list of input files
	/*
	cout << DELIM << endl;
	cout << " RAND: " << endl;
	cout << datafiles[i] << endl;
	cout << DELIM << endl;
	cout << " ALL: " << endl;
	// /*
	for (i=0;i<datafiles.size();i++){
		cout << datafiles[i] << endl;
	}
	cout << DELIM << endl;
	// */
	
	return(i);
}


//-----------------------------------------
//-  read files

  /*
get next line, create time_entry, add to history  */
int appliance::next_line(){
	if(!infile.is_open()){
		cerr << "-!!-Error: file not open\n";
		return(-1);
	}
	string L;
	bool is_valid = false;
	getline(infile, L);

	if (L.length() > 0){
		time_entry new_entry(L);
		is_valid = new_entry.is_valid;
		if (new_entry.is_valid){ history.push_back(new_entry); }
		else{ cout << "invalid. energy : " << new_entry.energy << endl; }
		Balance += new_entry.energy;
	}
	else{
		is_valid = false;
	}
	return is_valid;
}


  /*
read all remaining lines from infile, print to screen for testing  */
int appliance::read_all(){
	int flag = 2;
	int N = 0;
	while (flag > 1){
		flag = next_line();
		N++;
	}
	
	cout << "found [" << N << "] lines..." << endl;
	cout << "--printing history, size: [" << history.size() << "]" << endl;
	// dump_data();     //-dump all
	dump_nonempty(); //-reduce output
	
	cout << DELIM << endl;
	cout << " total energy used for the day: " << Balance << endl;
	cout << DELIM << endl;
	
	return(1);
}



//------------ end of appliance class ----------------------
