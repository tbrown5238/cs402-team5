/* ==========================================
*	FILE:	 run_test.cpp
*	PROJECT: Making Appliances Self Aware
*	AUTHOR:	 Travis Brown, Jason Derrick, Mark Bolles, Ryan Tungett
*	DATE:	 2016-10-18
*
*	* This program manipulates the device and appliance objects to simulate
*	  energy usage and communicate with other instances of this program.
*
========================================== */

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "time_entry.h"
#include "appliance.h"
#include "bidding.cpp"

// #include "comm.h"


using namespace std;


//=========================================
//==  Global Variables (settings)

//--input files

//-Car charger
string fname_CC_01  = "test_data/CC_set01.txt";
string fname_CC_02  = "test_data/CC_set02.txt";
string fname_CC_03  = "test_data/CC_set03.txt";
string fname_CC_04  = "test_data/CC_set04.txt";

//-HVAC
string fname_HV_01  = "test_data/HVAC_set01.txt";

//-Pool pump
string fname_PP_01  = "test_data/PP_set01.txt";
string fname_PP_02  = "test_data/PP_set02.txt";
string fname_PP_03  = "test_data/PP_set03.txt";
string fname_PP_04  = "test_data/PP_set04.txt";

//-Water heater
string fname_WH_01  = "test_data/WH_set01.txt";
string fname_WH_02  = "test_data/WH_set02.txt";
string fname_WH_03  = "test_data/WH_set03.txt";
string fname_WH_04  = "test_data/WH_set04.txt";

// vector <string> infile_list;


//--choose 1
// string input_file   = fname_CC_01;


//--other options, variables
string DELIM = "------------------------------------------";

//------------ end of global variables ---------------------


//=========================================
//==  program info, etc

void print_usage(){  cout << "Usage: ./run_test [type] [id]" << endl;  }

//------------ end of program info etc ---------------------



//=========================================
//==  Functions|Prototypes

int checkpoint(int timeA, int timeB) {
  return timeA - timeB;
}

//-wrappers for socket send/recieve functions
int my_recv(int server, char* buffer, int buf_len) {
	memset(buffer, 0, buf_len);
	recv(server, buffer, buf_len, 0);
	return(1);
}

int my_send(int server, string message) {
	cout << "-- sending: [" << message << "]" << endl;
	send(server, message.c_str(), message.length(), 0);
	return(1);
}

//------------ end of function definition ------------------


//=========================================
//==  begin "MAIN"
int main(int argc, char* argv[]){
	
	srand(time(NULL));
	//-----------------------------------------
	//-  Initialize, parse command line
	
	if(argc < 3){
		print_usage();
		return(-1);
	}
	string appl_type = argv[1];
	string myID = "0" + (string)argv[2];
	string my_info;  //-string to send to sim_server to store as metadata
	string input_file;
	double setLOAD;
	string Device_arg;
	
	if(appl_type == "CC"){
		my_info = "CarCharger; dev" + myID;
		input_file = fname_CC_01;
		Device_arg = "carcharger";
		setLOAD = 6.6;
	}else if(appl_type == "HV"){
		my_info = "HVAC; dev" + myID;
		input_file = fname_HV_01;
		Device_arg = "hvac";
		setLOAD = 0.25;
	}else if(appl_type == "PP"){
		my_info = "PoolPump; dev" + myID;
		input_file = fname_PP_01;
		Device_arg = "poolpump";
		setLOAD = 0.4;
	}else if(appl_type == "WH"){
		my_info = "WaterHeater; dev" + myID;
		input_file = fname_WH_01;
		Device_arg = "waterheater";
		setLOAD = 3.0;
	}else{
		cerr << "Error, appliance type [" << appl_type << "] not recognized" << endl;
		my_info = "CarCharger; dev" + myID;
		Device_arg = "carcharger";
		input_file = fname_CC_01;
		setLOAD = 6.6;
	}
	
	// cout << "Enter appliance power rating (LOAD) :  ";
	// cin >> setLOAD;
	
	cout << endl;
	
	appliance ME(setLOAD);
	
	
	//-----------------------------------------
	//-  integrate Device class
	
	cout << "----creating device----------------------" << endl;
	Device device (Device_arg.c_str());
	cout << device.power << "  " << device.failed_bids << endl;
	cout << "----^^ should NOT be an empty line ^^----" << endl;
	
	
	//-----------------------------------------
	//-  connect to simulation server
	
    int port = 10000;
    string host = "localhost";
	
//=========================
    //-use DNS to get IP address
    struct hostent *hostEntry;
    hostEntry = gethostbyname(host.c_str());
    if (!hostEntry) {
        cout << "No such host name: " << host << endl;
        exit(-1);
    }

      //-setup socket address structure
    struct sockaddr_in server_addr;
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr, hostEntry->h_addr_list[0], hostEntry->h_length);

      //-create socket
    int server = socket(PF_INET,SOCK_STREAM,0);
    if (server < 0) {
        perror("socket");
        exit(-1);
    }
	bool server_is_open = false;

// /*--
      //-connect to server
    if (connect(server,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        perror("connect");
        exit(-1);
    }
	else{
		server_is_open = true;
	}
//-------------------------connected
//=========================
	
	
	//-----------------------------------------
	//-  generate list of input files
	
	// ME.open_file(input_file);
	ME.add_datafile(input_file);
	// ME.add_datafile(input_file2);
	// ME.add_datafile(input_file3);
	// ME.add_datafile(input_file4);
	
	ME.random_file();
	
	
	
	//-----------------------------------------
	//-  Read input, dump output

	//-allocate buffer
	int buflen = 1024;
	char* buf = new char[buflen+1];
	
    //-recieve initial message ("getData")
	my_recv(server, buf, buflen);

	//-reply with device data
	my_send(server, my_info);


	//-continuously read a line from standard input and send to server
	string send_line;
/*
	while (getline(cin,send_line)) {
		//-write the data to the server
		my_send(server, send_line);

		//-read the response
		my_recv(server, buf, buflen);
		
		//-print the response
		cout << buf << endl;
		
		if(send_line == "exit"){ break; }
	}
//--*/
	
	int N = 0;
	
	char send_buff[1024];
	
	stringstream ss;
	while(ME.next_line()){
		N++;
		if(N%120 == 0){
			
			// char buf_N [33];
			// char buf_Bal [33];
			// itoa (N, buf_N, 10);
			// itoa (ME.Balance, buf_Bal, 10);
			// send_line = myID + "  " + buf_N + "  balance:  " + buf_Bal;
			
			ss.clear ();
			ss.str ("");
			ss << myID << "  " << N << "  balance:  " << ME.Balance;
			send_line = ss.str();

/*
			char buffer [50];
			int n, a=5, b=3;
			n=sprintf (buffer, "%d plus %d is %d", a, b, a+b);
*/
			// string str_N = itoa(N);
			// string str_Balance = (string)ME.Balance;
			// string str_Balance = itoa(ME.Balance);
			// send_line = myID + "  " + str_N + "  balance:  " + str_Balance;
			
			my_send(server, send_line);
			
			//-recieve acknowledgement
			my_recv(server, buf, buflen);
			cout << send_line << endl << buf << endl;
		
		}
		// cout.width(4);
		// cout << N << "  balance:  " << ME.Balance << endl;
		
		
		if(N>1450){ break; }  //-prevent infinite loop, just in case
	}
	send_line = "exit";
	my_send(server, send_line);
	// cout << DELIM << endl;
	// cout << "exited loop; N = " << N << endl;
	// cout << DELIM << endl;
	
	// ME.read_all();
	
	//-----------------------------------------
	//-  bookkeeping

    //-Close socket
    if(server_is_open){ close(server); }
	
	
	cerr << "happy exit" << endl << "hit enter to exit" << endl;
	cin.ignore();
	
	// return(1);
	return(0);
} // end MAIN

