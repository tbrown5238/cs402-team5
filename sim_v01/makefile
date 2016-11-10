CC = g++
CFLAGS = -g

DEPS = appliance.h comm.h time_entry.h
# OBJS = appliance.o comm.o run_test.o
OBJS = appliance.o run_test.o
EXES = run_test

all: run_test

# run_test: run_test.o appliance.o comm.o bidding.cpp time_entry.h
# run_test: $(OBJS) $(DEPS)
run_test: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

run_test.o: run_test.cpp appliance.h bidding.cpp time_entry.h
	$(CC) $(CFLAGS) -c run_test.cpp

appliance.o: appliance.cpp appliance.h time_entry.h
	$(CC) $(CFLAGS) -c appliance.cpp

comm.o: comm.cpp comm.h
	$(CC) $(CFLAGS) -c comm.cpp
# %.o: %.c $(DEPS)
# .cpp.o:
#	$(CC) $(CFLAGS) -c $<

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