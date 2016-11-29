#ifndef Q_LEARNING_H
#define Q_LEARNING_H

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <iomanip>

using namespace std;

class Q_learning{
public:
	Q_learning(){
		gamma = 0.5;
		// epsilon = 0.7;
		epsilon = 0.4;
		epsilon_min = 0.25;
		state = 0;		

		environment.resize(2880, vector<double>(2880));
		brain.resize(2880, vector<double>(2880));
		// p.resize(d2, vector<float>(d1,0)
		
		initialize_environment();
		initialize_brain();
	}
	~Q_learning(){ save_brain(); }  //-shouldn't be anything to delete, but make sure to save progress

	//-initialize
	void initialize_environment();
	void initialize_brain();
	void save_brain();
	
	//-execute
	int get_decision(double, double, int);
	
	//-internal
	int choose_action(int);
	double max_next(int);
	int state;
	int action;
	
	
	//-for testing
	void print_32();


protected:
	double gamma;       //gamma parameter to determine how important future goals are
	double epsilon;     //encourages exploration
	double epsilon_min; //-minimum exploration coefficient (after "answer" has been found)

	// vector<vector<int> > environment(2880, vector<int>(2880));
	// vector<vector<double> > brain(2880, vector<double>(2880));
	vector<vector<double> > environment;
	vector<vector<double> > brain;

};

#endif
