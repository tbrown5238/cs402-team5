CC = g++
# CFLAGS = -g -std=c++11 -Wall
CFLAGS = -g -std=c++11

# DEPS = appliance.h bidding.h comm.h time_entry.h
# OBJS = appliance.o bidding.o comm.o run_test.o
DEPS = appliance.h comm.h time_entry.h
OBJS = appliance.o comm.o run_test.o
EXES = run_test

all: run_test

run_test: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f run_test $(OBJS) $(EXES)
	
#####################################################
#####################################################
	
#	$@  name of file to be made
#	$?  names of changed dependents

##	$@: $?


#	$<  name of related file that caused the action
#	$*  prefix shared by target and dependent files

#	*.cpp  includes every file that ends in .cpp


#	%  wildcard

#####################################################
