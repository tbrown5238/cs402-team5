#!/bin/sh
#-----------------------------------------
#	gogogo.sh
#	Travis Brown		2016-11-22
#-----------------------------------------
#	script to start several instances of the "run_test" executable.
#	Created to simplify testing process
#-----------------------------------------

# python sim_server/System_Sim.py > out_python.txt &
# python sim_server/System_Sim.py &
# sleep 1

# ./run_test WH > /dev/null &
./run_test WH > out_WH.txt &
sleep 1
# ./run_test PP > /dev/null &
./run_test PP > out_PP.txt &
sleep 1
# ./run_test CC > /dev/null &
./run_test CC > out_CC.txt &
sleep 1
# ./run_test HV > /dev/null &
# ./run_test HV > /dev/null
./run_test HV > out_HV.txt
sleep 1
