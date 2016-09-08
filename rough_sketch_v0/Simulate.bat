@ECHO OFF
REM Batch file
REM this will be a file to initiate the simulation

REM step1 run simulation

REM step2 run device
REM step3 repeat step2 desired number of times

REM start python C:\DATA\Files_F\github\cs402-team5\rough_sketch_v0\System_Sim.py
REM start python C:\DATA\Files_F\github\cs402-team5\rough_sketch_v0\Appliance.py

start python System_Sim.py
start python Appliance.py pump 1
wait 3
start python Appliance.py therm 2

