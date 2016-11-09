#include <iostream>
#include <stdio.h>
#include <string>
#include <cstring>
#include <vector>
#include <time.h>

using namespace std;

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


Device::Device(string name){
  is_initialized = false;
  if(strcmp(name.c_str(), "waterheater")==0){
    power = 3.0;
    credits = 100000.0;
    income = 0.0;
    failed_bids = 0.0;
    desperation = 0.0;
    is_initialized = true;

  }else if(strcmp(name.c_str(), "hvac")==0){
    power = 0.25;
    credits = 100000.0;
    income = 0.0;
    failed_bids = 0.0;
    desperation = 0.0;
     is_initialized = true;

  }else if(strcmp(name.c_str(), "carcharger")==0){
    power = 6.6;
    credits = 100000.0;
    income = 0.0;
    failed_bids = 0.0;
    desperation = 0.0;
    is_initialized = true;

  }else if(strcmp(name.c_str(), "poolpump")==0){
    power = 0.4;
    credits = 100000.0;
    income = 0.0;
    failed_bids = 0.0;
    desperation = 0.0;
    is_initialized = true;

  }else{
    cout<<"Syntax Error: ./bidding <waterheater|hvac|carcharger|poolpump>"<<endl;
    return; 
  
  }
}




double Device::get_bid(int stage){

  double bid;

  if(stage = 1){
    bid = power;   //bids how much power they use
    if(credits - bid < 0){  
      return 0.0;			
    }else
      return bid;

  }else if(stage = 2){
    bid = power*(1+desperation); //bids power * how desperate they are
    if(credits - bid < 0){
      return 0.0;
    }else
      return bid;

  }else
    bid = power*(1+desperation)*1.5;  //bids power * desperation * 1.5
    if(credits - bid < 0){
      return 0.0;
    }else
      return bid;

}

void Device::set_desperation(){

//look at the previous 14 minutes or the last time the device was on and determine how bad it needs to be turned on. Also look at failed bids.
  
desperation += failed_bids*0.1;   //will make more complicated using last time on


}

void Device::bidding(){

  struct Bidders{
    string address;
    double bid;
    double power;

  };

  //vector<Bidders> bidders;
  
  double first_bid, second_bid, third_bid;
  first_bid = get_bid(1);
  //send your bid and receive bids from other devices
  //put all bids in a vector starting with your own
  vector<double> all_bids;
  all_bids.push_back(first_bid);
  //all_bids.push_back(all other bids);
  
/*
  if(first_bid == 0)
    send your bid and receive bids from other devices
    second_bid = 0;
    send your bid and receive bids from other devices
    third_bid = 0;
    send your bid and receive bids from other devices
  
  }else if(first_bid != 0 && all other devices bids == 0){
    you win the slot and ALL other bidding is stopped for the minute and subtract credits

  }else if(first_bid != 0 && other bids != 0){
    determine who has the highest bid 
    
    if(first_bid > all other bids){
      second_bid = first bid;
    
    }else if(first bid < other bids){
      second_bid = get_bid(2)
    }

    //send your bid and receive bids from other devices
    //put all bids in a vector starting with your own
    all_bids.clear(); 
    all_bids.push_back(second_bid);
    all_bids.push_back(all other bids);

    if(second_bid > all other bids){
      third_bid = second_bid;
    
    }else if(second_bid < other bids){
      third_bid = get_bid(3)
    }
    
    //send your bid and receive bids from other devices
    //put all bids in a vector starting with your own
    all_bids.clear(); 
    all_bids.push_back(third_bid);
    all_bids.push_back(all other bids);

    if(third_bid > all other bids){
      you win the time slot

    }else if(third < other bids){
      ask device to shed load if already on
      you lose and you update your failed bid counter
      device.failed_bids += 1.0;
    }

  }

*/
}

struct Time {
     
    int hour;
    int minute;
    int second;
    vector<string> owner;
};
/*
Creates a vector of time that represent the beginning of each time
slot. So if it was 09:01:37 then it would fall under the 09:01:00 slot.
*/
vector<Time> create_slots(){

  vector<Time> times;

  times.resize(1440);

  for(int i=0; i<1440; i++){
    if(i==0){
      times[i].second=00;
      times[i].minute=00;
      times[i].hour=00;
    
    }else{
      if(times[i-1].minute < 59){
        times[i].minute=times[i-1].minute+1;
        times[i].hour=times[i-1].hour;
      }else
        times[i].hour=times[i-1].hour+1;

    }
  }
  
  return times;
}

/*
This is supposed to determine who the owners are by looking at the 
current time and checking that time slot.
*/
int determine_owner(vector<Time> times){
  
  time_t rawtime;
  struct tm * ptm;
  time(&rawtime);
  
  ptm = localtime ( &rawtime );    //current time
  int hour = (ptm->tm_hour);    
  int minute = ptm->tm_min;
  //int second = ptm->tm_sec;
  for(int i=0;i<1440;i++){
    if(hour == times[i].hour){
      if(minute == times[i].minute){      
//        printf("%02d:%02d:%02d %s %s\n",slots[i].hour,slots[i].minute,slots[i].second, slots[i].owner[0].c_str(), slots[i].owner[1].c_str());
        break;
      }        
    }
  }
  return 0;
}

//Prints the time slots and their owners
int print_slots(vector<Time> times){

  for(int j=0;j<1440;j++){
    printf("%02d:%02d:%02d\n",times[j].hour,times[j].minute,times[j].second);
    //cout<<"   "<<slots[j].owner[0]<<"   "<<slots[j].owner[1]<<endl;
  }
  return 0;
}




//Bidding
//Am I on? Am I manually overridden?
//Remember to update failed bid variable after each minute
//Can I bid? Should I bid?

/*First Bidding stage

sends out a base bid determined by energy usage
if it is the only one to bid a non-zero number then it wins

Calculate Bid
Send bid to all devices
Receive bid from all devices
determine who won 
if only one non-zero bid then that device wins
else bid again

*/

/*Second Bidding stage

sends out a bid determined by energy usage and desperation

calculate second bid
send to all devices
receive from all devices
determine who won
bid again

*/

/*Third Bidding stage

sends out a bid determined by energy usage, desperation, and failed bid 
who ever wins this pays the amount and gets first slot
Repeat until there are no more devices bidding or until cap is met

calculate final bid
send bid to all devices
receive bid from all devices
determine who won
subtract bid from total money winner has.
update time slot with owner of slot



*/
