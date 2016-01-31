#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

#include<iostream>
#include <string>
#include "Pkt_Def.h"

// By: Igor Naperkovskiy and Joao Rodrigues
using namespace std;

int main() {

	SOCKET ServerSocket, ConnectionSocket;
	char IP[128] = { "127.0.0.1" };
	int Port = 5000;

	//PktDef PacketObj;
	
	while (1){
		PktDef PacketObj;
		int status = 0;
		if (PacketObj.Listen(ServerSocket, IP, Port) != 0) {
			if (PacketObj.Accept(ServerSocket, ConnectionSocket) != 0) {

				PacketObj.SetInfo();
				status = PacketObj.Send(ConnectionSocket);
				if (status == 5){
					PacketObj.ReceivePkt(ConnectionSocket);
				}				
				PacketObj.CloseSocket(ConnectionSocket);
				PacketObj.CloseSocket(ServerSocket);
				PacketObj.WinsockExit();
				if (status == 5){
					PacketObj.PrintInfo();
				}
			}
		}
		else {
				cout << " Connection Error - Exiting..." << endl;
		}
	}
		return 0;

		
	}