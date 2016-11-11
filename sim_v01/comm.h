#ifndef COMM_H
#define COMM_H

// #include <arpa/inet.h>
// #include <errno.h>
#include <netdb.h>
#include <stdlib.h>
// #include <sys/types.h>
// #include <sys/socket.h>
#include <unistd.h>

#include <string.h>
#include <string>

#include <iostream>
#include <fstream>
#include <sstream>

#include "bidding.h"

using namespace std;


//=========================================
//==  comm
class comm{
public:
	comm();
	~comm(){
		delete buf;
		c_close();
	}
	
	//-establish connection
	bool c_connect();
	
	//-close socket connection
	void c_close(){ if(server_is_open){ close(server); } }

	//-basic send/recieve
	int c_send(string MSG);
	string c_recv();

	//-communicate bidding information
	int send_bid(int ID, double bid);
	int get_bid();

protected:
	//-basic metadata
    int port;
    string host;
    int server;
	bool server_is_open;
	
	//-socket structures
    struct hostent *hostEntry;
    struct sockaddr_in server_addr;
	
	
	//-send/recieve
	int buflen;
	char* buf;
	char send_buff[1024];

};


#endif
