#include <vector>
#include <iostream>
#include <stdlib.h>
#include <iomanip>

#include "q_learning.h"

using namespace std;

int main(void){
	srand(123);
	
	Q_learning Q;
	// Q.initialize_environment();
	// Q.initialize_brain();
	Q.print_32();
	
	for(int i=0; i<100; i++){  
		Q.Episode(0);
	}

	Q.save_brain();
	Q.print_32();
	
	
	return{0};
}

