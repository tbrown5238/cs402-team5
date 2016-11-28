#include <vector>
#include <iostream>
#include <stdlib.h>

using namespace std;

void initialize_environment();
void initialize_brain();
int choose_action(int);
double max_next(int);
int Episode(int);
void save_brain();
void print_32();
