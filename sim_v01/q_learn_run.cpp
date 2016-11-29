#include <vector>
#include <iostream>
#include <stdlib.h>
#include <iomanip>

#include "q_learning.h"

using namespace std;

int main(void){
	srand(123);
	
	Q_learning Q;
	
	Q.get_decision(4, 7, 1);
	
	Q.save_brain();
		
	Q.print_32();
	
	
	return{0};
}
