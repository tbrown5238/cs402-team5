#ifndef COMM_H
#define COMM_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>


using namespace std;


//=========================================
//==  comm


class comm{
public:
	comm();
	~comm(){ delete buf; }
	
	//-establish connection
	bool connect();

	//-basic send/recieve
	int send(string MSG);
	string recv();

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
	int buflen = 1024;
	char* buf;
	char send_buff[1024];

};


#endif
