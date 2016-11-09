#ifndef TIME_ENTRY_H
#define TIME_ENTRY_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
// #include <regex>

using namespace std;

//=========================================
//==  Time_entry

class time_entry{
public:
	time_entry(string Line) {
		L = Line;
		energy = 0;
		
		size_t T = L.find('\t');
		if (T != std::string::npos){
			subL = L.substr (T, L.length()-1);
			is_valid = true;
		}
		else{
			cerr << " ~no tab~ [" << L << "]" << endl;
			is_valid = false;
		}
		L = subL;
		energy = atof(subL.c_str());
		//--^^ could use strtok ^^
		
		strptime(Line.c_str(), "%H:%M:%S", &tm);
		t = mktime(&tm);
	}
	~time_entry(){}


	bool is_valid;
	string L;
	string subL;
	float energy;    //-power consumption of the appliance
	
	time_t t;
	struct tm tm;
	// tm * tm_obj;
};


#endif
