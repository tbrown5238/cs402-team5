/* ==========================================
*	FILE:	 comm.cpp
*	PROJECT: Making Appliances Self Aware
*	AUTHOR:	 Travis Brown, Jason Derrick, Mark Bolles, Ryan Tungett
*	DATE:	 2016-10-18
*
*	* This class is used to facilitate communications between devices.
*	  Currently, it communicates through a central server, but can
*	  be modified to communicate directly with other devices.
*
========================================== */

#include "comm.h"

using namespace std;


//=========================================
//==  comm


//-constructor
comm::comm(){
	//-initialize basic metadata
    host = "localhost";
    port = 10000;
    
    //-use DNS to get IP address
	hostEntry = gethostbyname(host.c_str());
	if (!hostEntry) {
		cout << "No such host name: " << host << endl;
		exit(-1);
	}

	//-setup socket address structure
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	memcpy(&server_addr.sin_addr, hostEntry->h_addr_list[0], hostEntry->h_length);

    //-create socket
    server = socket(PF_INET,SOCK_STREAM,0);
    if (server < 0) {
        perror("socket");
        exit(-1);
    }
	server_is_open = false;
	
	//-initialize send/recieve variables
	buflen = 1024;
	buf = new char[buflen+1];
}


//-establish connection
bool comm::c_connect(){
	//-connect to server
	cout << "..connecting @" << host << "." << port << endl;
	if (connect(server,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
		perror("connect");
		exit(-1);  //? return false, handle exit in main?
	}
	else{
		server_is_open = true;
	}
	
	return server_is_open;
}


//-basic send
int comm::c_send(string MSG) {
	cout << " <<   [" << MSG << "]" << endl;
	send(server, MSG.c_str(), MSG.length(), 0);
	return(1);
}

//-basic recieve
string comm::c_recv() {
	memset(buf, 0, buflen);
	recv(server, buf, buflen, 0);
	// return(string(buf));
	string R = string(buf);
	R.erase(R.end()-1);
	cout << "   >> [" << R << "]" << endl;
	return(R);
}

//-communicate bidding information
int comm::send_bid(int ID, double bid){
	
	
	return(1);
	
}


int comm::get_bid(){
	
	
	return(1);
	
}


//------------ end of comm class ---------------------------

