#include <vector>
#include <iostream>

using namespace std;

double gamma = 0.5;       //gamma parameter to determine how important future goals are


vector<vector<int> > environment(2880, vector<int>(2880));
vector<vector<int> > brain(2880, vector<int>(2880));

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

void print_32(){
  for(int i=0; i<32; i++){
    for(int j=0; j<32; j++){
      cout<<environment[i][j]<<" ";
    }
    cout<<endl;
  }
}

int main(void){

  initialize_environment();
//  print_32();
}

