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
	
	Q.Episode(0);

	Q.print_32();
	return{0};
}

