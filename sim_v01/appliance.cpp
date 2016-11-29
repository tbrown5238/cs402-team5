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
Appliance::Appliance(string type){

	//-----------------------------------------
	//-  create list of possible input files
	//-    based on appliance type
	if(type == "CC"){
		//-Car charger
		datafiles.push_back("test_data/CC_set01.txt");
		datafiles.push_back("test_data/CC_set02.txt");
		// datafiles.push_back("test_data/CC_set03.txt");
		// datafiles.push_back("test_data/CC_set04.txt");
		
		/*
		datafiles.push_back("test_data/miniCC_set01a.txt");
		datafiles.push_back("test_data/miniCC_set01b.txt");
		// */
		LOAD = 6.6;
	}else if(type == "HV"){
		//-HVAC
		datafiles.push_back("test_data/HVAC_set01.txt");
		// LOAD = 0.25;
		LOAD = 1.0;
	}else if(type == "PP"){
		//-Pool pump
		datafiles.push_back("test_data/PP_set01.txt");
		datafiles.push_back("test_data/PP_set02.txt");
		// datafiles.push_back("test_data/PP_set03.txt");
		// datafiles.push_back("test_data/PP_set04.txt");
		LOAD = 0.4;
	}else if(type == "WH"){
		//-Water heater
		datafiles.push_back("test_data/WH_set01.txt");
		datafiles.push_back("test_data/WH_set02.txt");
		// datafiles.push_back("test_data/WH_set03.txt");
		// datafiles.push_back("test_data/WH_set04.txt");
		LOAD = 3.0;
	}else{
		cerr << "Error, appliance type [" << type << "] not recognized" << endl;
		//-default to car charger to avoid unexpected exit
		datafiles.push_back("test_data/CC_set01.txt");
		LOAD = 6.6;
	}

	Balance = 0.0;
	spend = 0.0;
	
	avg_L = 0;
	avg_N = 0;
	
	ID = "00";
	minutes_standby = 0;
	needs_to_run = false;
	
	prev_time = 0;
	current_state = OFF;
}


//-----------------------------------------
//-  input files

  /*
open file by string  */
int Appliance::open_file(string filename){
	//-make sure to close existing file
	if(infile.is_open()){ infile.close(); }
	
	//---?? should this file be added to datafiles??
	current_fname = filename;
	//-verbose:
	// cerr << DELIM << endl;
	// cerr << "<" << ID << ">  opening: " << current_fname << endl;
	
	if(current_fname.size()<24){
		cerr << (DELIM+"\n<"+ID+">  opening  "+current_fname+"   <"+ID+">\n"+DELIM) << endl;
	}
	else{
		cerr << (DELIM+"\n<"+ID+"> opening  "+current_fname+"  <"+ID+">\n"+DELIM) << endl;
	}
	
	infile.open(filename.c_str());
	if(!infile.is_open()){
		cerr << "-!!-Error: file not open\n<>  " << current_fname << endl;
		return(-1);
	}
	return(1);
}

  /*
open file by index  */
int Appliance::open_file(int f_index){
	//-if negative (invalid) argument, open random file instead
	if(f_index < 0){
		cerr << "-!!-Error: open_file(index) : cannot open negative index, opening random instead\n";
		int f_num;
		f_num = random_file();
		return f_num;
	}

	//-make sure index is within range
	if(f_index >= datafiles.size()){
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
int Appliance::random_file(){
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
int Appliance::next_line(){
	if(!infile.is_open()){
		cerr << "-!!-Error: file not open\n";
		return(-1);
	}
	// else{ /*open next file*/ } //??
	
	string L;
	bool is_valid = false;
	// getline(infile, L);

	int try_file = 0;
	do{
		getline(infile, L);
		if (try_file > 10){
			return(false);
		}
		if (L.length() > 0){
			time_entry new_entry(L);
			is_valid = new_entry.is_valid;
			if (new_entry.is_valid){
				//-entry is valid, add to history
				history.push_back(new_entry);
				//-update Balance and spend variables
				Balance += new_entry.energy;
				to_spend();
			}
			else{ cout << "invalid. energy : " << new_entry.energy << endl; }
			// Balance += new_entry.energy;  //-moved outside loop; here Balance will update regardless of is_valid
		}
		else{
			is_valid = false;
			/*open next file*/
			try_file++;
			// open_file("test_data/miniCC_set01b.txt");
			random_file();
			
			//?recursion? ... no, use do-while
		}
	}while(!is_valid);
	
	update_state();
	//-returns true if a valid entry was found (if continuous, should always be true by this point)
	return is_valid;
}


  /*
read all remaining lines from infile, print to screen for testing  */
int Appliance::read_all(){
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


//-----------------------------------------
//-  operation/communication

  /*
check current state and energy balance to determine if appliance needs to run  */
void Appliance::update_state(){
	if(current_state == STANDBY){ minutes_standby++; }
	if(Balance > 0){
		if(current_state == OFF){ current_state = ON; }
		needs_to_run = true;
		// return(true);
		return;
	}
	else{
		spend = 0.0;
		current_state = OFF;
		needs_to_run = false;
		// return(false);
		return;
	}
}


  /*
attempt to enter standby state; reterns true on success  */
bool Appliance::enter_standby(){
	if(is_override()){
		cerr << "  [_!_STANDBY_!_]  " << endl;
		return(false);
	}
	else{
		current_state = STANDBY;
		cerr << "  [STANDBY]  " << endl;
		return(true);
	}
}


  /*
lifts "standby" mode, re-enters "on" state  */
bool Appliance::exit_standby(){
	if(Balance > 0){
		current_state = ON;
		cerr << "  [EXIT][" << minutes_standby << "]  " << endl;
		minutes_standby = 0;
		return(true);
	}
	else{
		current_state = OFF;
		cerr << "  [_!_EXIT_!_]  " << endl;
		return(false);
	}
}


  /*
checks current state to determine if energy was spent in the previous minute  */
double Appliance::spent(){
	if((current_state == ON) || (current_state == OVERRIDE)){
		//-current state was running, energy WAS spent
		return spend;
	}
	else{
		//-current_state = [OFF|STANDBY]
		//-appliance was not running, energy WAS NOT spent
		return(0.0);
		// Balance -= spent;
	}
}


  /*
determine how much energy will be consumed if device is allowed to run
does NOT modify Balance  */
void Appliance::to_spend(){
	if(Balance <= LOAD){
		spend = Balance*1.0;  // make sure it's not an int --??necessary??
		// Balance = 0.0;
	}
	else{
		spend = LOAD*1.0;
		// Balance -= spent;
	}
	return;
}


  /*
modify Balance to reflect upcoming energy consumption
DOES modify Balance  */
double Appliance::spend_energy(){
	//-Assumes spend has already been calculated
	/*
	... if spend has been calculated correctly, then it should just be:
	Balance -= spend
	because spend should never be greater than Balance
	*/
	if(current_state == STANDBY){
		//-device has shut off appliance, do not spend energy
		return(0.0);
	}
	if(current_state == OFF){
		//-device has shut off appliance, do not spend energy
		if(spend > 0){
			cerr << "--!!-- oops, spend > 0 during OFF" << endl;
		}
		return(0.0);
	}
	if(Balance <= LOAD){
		//- should mean that (Balance == spend)
		Balance = 0.0;
	}
	else{
		Balance -= spend;
	}
	return(spend);
}

//------------ end of appliance class ----------------------
