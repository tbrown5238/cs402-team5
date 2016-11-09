CC = g++
CFLAGS = -g

all: run_test

run_test: run_test.o appliance.o
	$(CC) $(CFLAGS) -o $@ $^

run_test.o: run_test.cpp appliance.h time_entry.h
	$(CC) $(CFLAGS) -c run_test.cpp

appliance.o: appliance.cpp appliance.h time_entry.h
	$(CC) $(CFLAGS) -c appliance.cpp

device.o: device.cpp device.h time_entry.h
	$(CC) $(CFLAGS) -c device.cpp
# .cpp.o:
#	$(CC) $(CFLAGS) -c $<

clean:
	rm -f run_test run_test.o appliance.o
	
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