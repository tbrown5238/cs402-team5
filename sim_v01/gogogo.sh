#!/bin/sh
#-----------------------------------------
#	gogogo.sh
#	Travis Brown		2016-11-22
#-----------------------------------------
#	script to start several instances of the "run_test" executable.
#	Created to simplify testing process
#-----------------------------------------

# python sim_server/System_Sim.py > out_python.txt &
python sim_server/System_Sim.py &

sleep 1

./run_test CC 1 > out_CC.txt &

./run_test WH 2 > out_WH.txt &
