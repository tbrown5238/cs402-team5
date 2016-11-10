#ifndef bidding_h
#define bidding_h

#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <vector>
#include <time.h>

class Device{
  public:
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

    double get_bid(int);
    Device(string);
    void set_desperation();
    void bidding();
};
