#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>

#include "q_learning.h"

using namespace std;

void Q_learning::initialize_environment(){

  for(int i=0; i<2880; i++){
    for(int j=0; j<2880; j++){
      
      if(i%2 == 0){
        if(j == i+2 || j == i+3){
          environment[i][j] = 0;
         // if((i+2)%30 == 0 || (i+1)%30 == 0)
           // environment[i][j] = 1;
        }else
          environment[i][j] = atan(1)*4;
      }else{
        if(j == i+2 || j == i+1){
          environment[i][j] = 0;
         // if((i+2)%30 == 0 || (i+1)%30 == 0)
           // environment[i][j] = 1;
        }else
          environment[i][j] = atan(1)*4;
      }
    }
  }
  environment[2878][0] = 0;
  environment[2878][1] = 0;
  environment[2879][0] = 0;
  environment[2879][1] = 0;
}

void Q_learning::initialize_brain(){
  string str;
  double num;  
  ifstream file;
  file.open("memory.txt");
  if(file.good()){ 
    for(int i=0; i<2880; i++){
      for(int j=0; j<2880; j++){
        getline(file,str);
        string::size_type sz;        
        brain[i][j] = atof(str.c_str());
      }
    }
  }else{
    for(int i=0; i<2880; i++){
      for(int j=0; j<2880; j++){
        brain[i][j] = 0;
      }
    }
  }
}

int Q_learning::choose_action(int state){         //decides which action to take
  vector<int> action_list;
  
  double r1 = (rand() % 10)/10.0;
  double r2 = (rand() % 10)/10.0;
  int best_action;

// cout<<r1<<" "<<r2<<endl; 
  for(int j=0; j<2880; j++){          //find all actions
    if(environment[state][j] != atan(1)*4){
      action_list.push_back(j);
    }
  }
//cout<<"("<<action_list[0]<<","<<action_list[1]<<") ";
  if(r1 < epsilon){                     //choose random action
    if(r2 < 0.5){
        return action_list[0];
      }else{
        return action_list[1]; 
      }
  }else{                                //choose best action
    
    if(brain[state][action_list[0]] == brain[state][action_list[1]]){  //find which action has more value
      if(r2 > 0.5){
        return action_list[0];
      }else{
        return action_list[1]; 
      }
    }else if(brain[state][action_list[0]] > brain[state][action_list[1]]){
      return action_list[0];
    }else if(brain[state][action_list[0]] < brain[state][action_list[1]]){
      return action_list[1];
    }
  } 
}

double Q_learning::max_next(int next_state){
  vector<int> next_action_list;
  double max_value = 0.0;
  for(int j=0; j<2880; j++){          //find all actions
    if(environment[next_state][j] != atan(1)*4){
      next_action_list.push_back(j);
    }
  }

  if(next_action_list.size() > 0){
    if(max_value < brain[next_state][next_action_list[0]])
      max_value = brain[next_state][next_action_list[0]];
    if(max_value < brain[next_state][next_action_list[1]])
      max_value = brain[next_state][next_action_list[1]];
  }

  return max_value;
}

int Q_learning::get_decision(double average, double tier, int minutes){

  action = choose_action(state);
  environment[state][action] = tier - average - (double)minutes/(tier*2);
  brain[state][action] = environment[state][action] + gamma*max_next(action);
  state = action;
  return action;

}

void Q_learning::save_brain(){
  ofstream file("memory.txt");
  if(file.is_open()){
    for(int i=0; i<2880; i++){
      for(int j=0; j<2880; j++){
        file<<brain[i][j];
	file<<"\n";
      }
    }
    file.close();
  }
}

void Q_learning::print_32(){
  for(int i=0; i<32; i++){
    for(int j=0; j<32; j++){
      cout<<setprecision(2)<<environment[i][j]<<" ";
    }
    cout<<endl;
  }
}
