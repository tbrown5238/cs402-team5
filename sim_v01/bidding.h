#ifndef BIDDING_H
#define BIDDING_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <vector>
#include <time.h>
#include "comm.h"

using namespace std;

class Device{
  public:
	//-constructor
    Device(string);
	
	string name;
	double power;           //power consumption hard coded into device name
	double credits;
	double income;          //hard coded
	int time_used;          //how many time slots it has already used                   
	int status;             //on/off
	double failed_bids;     //amount of failed bids
	vector<int> past;       //stores the previous 14 minutes
	double desperation;
	time_t last_time_on;
	bool is_initialized;

	//-functions
	double get_bid(int);
	void set_desperation();
	void bidding(comm, int, int);
};

struct Bidders{
    int address;
    double bid;

  };

#endif
