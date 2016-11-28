#include <vector>
#include <iostream>
#include <stdlib.h>
#include <iomanip>

using namespace std;

double gamma = 0.5;       //gamma parameter to determine how important future goals are
double epsilon = 0.7;     //encourages exploration

vector<vector<int> > environment(2880, vector<int>(2880));
vector<vector<double> > brain(2880, vector<double>(2880));

void initialize_environment(){

  for(int i=0; i<2880; i++){
    for(int j=0; j<2880; j++){
      
      if(i%2 == 0){
        if(j == i+2 || j == i+3){
          environment[i][j] = 0;
          if((i+2)%30 == 0 || (i+1)%30 == 0)
            environment[i][j] = 1;
        }else
          environment[i][j] = 2;
      }else{
        if(j == i+2 || j == i+1){
          environment[i][j] = 0;
          if((i+2)%30 == 0 || (i+1)%30 == 0)
            environment[i][j] = 1;
        }else
          environment[i][j] = 2;
      }
    }
  }
}

void initialize_brain(){
  for(int i=0; i<2880; i++){
    for(int j=0; j<2880; j++){
      brain[i][j] = 0;
    }
  }
}

int choose_action(int state){         //decides which action to take
  vector<int> action_list;
  
  double r1 = (rand() % 10)/10.0;
  double r2 = (rand() % 10)/10.0;
  int best_action;

// cout<<r1<<" "<<r2<<endl; 
  for(int j=0; j<2880; j++){          //find all actions
    if(environment[state][j] < 2){
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

double max_next(int next_state){
  vector<int> next_action_list;
  double max_value = 0.0;
  for(int j=0; j<2880; j++){          //find all actions
    if(environment[next_state][j] < 2){
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

int Episode(int state){
  int action;  
  do{
    action = choose_action(state);
    brain[state][action] = environment[state][action] + gamma*max_next(action); //reward + future reward
//cout<<state<<" ";
  state=action;
  }while(state<2878);
 

}

void print_32(){
  for(int i=0; i<32; i++){
    for(int j=0; j<32; j++){
      cout<<setprecision(2)<<brain[i][j]<<" ";
    }
    cout<<endl;
  }
}

int main(void){
  srand(123);
  initialize_brain();
  initialize_environment();
  Episode(0);

//  print_32();
}

