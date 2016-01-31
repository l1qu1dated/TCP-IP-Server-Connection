#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include "Pkt_Def.h"
#include <iostream>


// By: Igor Naperkovskiy and Joao Rodrigues
using namespace std;

PktDef::~PktDef() {
	delete[] body;
	body = nullptr;
}

void PktDef::SetInfo() {
	delete[] body;
	body = nullptr;
	bool tr = true;

	while (tr){ //loop start
		int AuxInt;
		cout << "What is the packet ID(15 DRIVE, 5 STATUS, 0 SLEEP)? ";
		cin >> AuxInt; //enter packet ID
		head.id = AuxInt; 

		if (AuxInt == 15){ // if id is drive
			cout << "What is the body size(no more than 10): ";
			cin >> AuxInt; //enter number of commands
			head.size = AuxInt;
			AuxInt = AuxInt * 2;
			int size = AuxInt;
			
			if (head.size <= 10){ //no more than 10 commands

				body = new Commands[head.size]; //allocates memory for commands

				for (int i = 0; i < head.size; i++){
					cout << "Enter the command (FORWARD[1],BACKWARD[2],LEFT[3],RIGHT[4]): ";
					cin >> AuxInt; //enter directio
					body[i].direction = AuxInt;

					if (AuxInt <= 4 && AuxInt >= 1){
						cout << "Enter Duration: ";
						cin >> AuxInt; //enter duration
						body[i].duration = AuxInt;
					}
					else {
						i--;
						cout << "Commands can only go from 1 - 4" << endl;
					}
				}

				unsigned int parity = 0; 
				unsigned char *temp; //temorary variable to calclate parity

				temp = new unsigned char[(head.size * 2) + 2]; //alocate memory for temp
				temp[0] = head.id;
				temp[1] = head.size;

				for (int i = 0; i < head.size; i++)
					temp[2 + i] = body[i].direction; 

				for (int i = 0; i < head.size; i++)
					temp[2 + head.size + i] = body[i].duration;

				for (int i = 0; i < size + 2; i++){ //loop that calculates parity

					while (temp[i] > 0){

						if (((int)temp[i] & 1) == 1) parity++;

						temp[i] >>= 1;
					}
				} // end of loop

				trailer = parity; 

				cout << "Trailer: " << (int)trailer << endl;
				tr = false;
			}
			else{
				cout << "no more then 10 commands" << endl;
			}
		}
		else if (AuxInt == 5){ // if command is status
			head.size = 0; 
			body = new Commands[0];
			trailer = 2; //parity is 2
			tr = false;
		}
		else if (AuxInt == 0){ // if command is sleep
			head.size = 0;
			body = new Commands[0];
			trailer = 0;
			tr = false;
		}
		else {
			cout << "incorrect command" << endl;
		}
	}//end of loop
}

int PktDef::Listen(SOCKET &ServerSocket, char * IP, int Port) {
	//starts Winsock DLLs   
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	//create server socket
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET) {
		WSACleanup();
		return 0;
	}

	//binds socket to address
	struct sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;
	//Listen on local host
	SvrAddr.sin_addr.s_addr = inet_addr(IP);
	//Listen to port 5000 for robot
	SvrAddr.sin_port = htons(Port);
	if (bind(ServerSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
	{
		closesocket(ServerSocket);
		WSACleanup();
		return 0;
	}

	//listen on a socket
	if (listen(ServerSocket, 1) == SOCKET_ERROR) {
		closesocket(ServerSocket);
		WSACleanup();
		return 0;
	}

	cout << "Waiting for robot connection..." << endl;

	return 1;

}

int PktDef::Accept(SOCKET &ServerSocket, SOCKET &ConnectionSocket) {
	if ((ConnectionSocket = accept(ServerSocket, NULL, NULL)) == SOCKET_ERROR) {
		closesocket(ServerSocket);
		WSACleanup();
		return 0;
	}

	cout << "Connection to robot established!" << endl;
	return 1;
}

int PktDef::Send(SOCKET &ConnectionSocket){
	unsigned char *TxBuffer; // create buffer pointer
	
	TxBuffer = new unsigned char[(head.size*2) + 3]; //allocate proper amount of space

	TxBuffer[0] = head.id; 
	TxBuffer[1] = head.size;
	int b = 0; //iterates through structure
	for (int i = 0; i < head.size * 2; i++){
		TxBuffer[2 + i] = body[b].direction;
		TxBuffer[3 + i] = body[b].duration;
		b++; i++;
	}

	TxBuffer[(head.size * 2) + 2] = trailer;

	send(ConnectionSocket, (char *)TxBuffer, (head.size * 2) + 3, 0); // send to robot

	return (int)head.id;

}

void PktDef::CloseSocket(SOCKET &Socket){
	closesocket(Socket);
}


void PktDef::WinsockExit(){
	WSACleanup();
}

void PktDef::ReceivePkt(SOCKET &ConnectionSocket){
	delete[] body;

	body = nullptr;

	char RxBuffer[128] = {};
	if (recv(ConnectionSocket, RxBuffer, sizeof(RxBuffer), 0) > 0) {
		head.id = RxBuffer[0];
		head.size = RxBuffer[1];
		body = new Commands[RxBuffer[1]];
		for (int i = 0; i < RxBuffer[1]; i++){
			body[i].direction = RxBuffer[2 + i];
		}

		trailer = RxBuffer[2 + RxBuffer[1]];
	}
}

void PktDef::PrintInfo(){

	cout << "Received PacketID: " << (int)head.id << endl;
	cout << "Receive CmdListSize: " << (int)head.size << endl;
	cout << "Received Status: ";
	for (int i = 0; i < head.size; i++)
		cout << (int)body[i].direction << " ";
	cout << endl << "Received Parity: " << (int)trailer << endl;
}
