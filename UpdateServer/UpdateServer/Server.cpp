#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include "FileHelper.h"
#include <sstream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

int requestCount = 0;
int requestSinceUpdate = 0;
const int STRLEN = 256;
SOCKET		mySocket;
const char FILENAME[] = "data.bin";
const char IPADDR[] = "127.0.0.1";
const int  PORT = 50000;
const int  QUERY = 1;
const int  UPDATE = 2;
const int HOTSWAP = 5;
WSADATA		wsaData;
SOCKET		listenSocket;
SOCKET		acceptSocket;
SOCKADDR_IN	serverAddr;

int num1 = 0;
int num2 = 0;

void cleanup(SOCKET socket)
{
	closesocket(socket);
	WSACleanup();
	cout << "\tConnection closed";
}

int getLocalVersion()
{
	ifstream dataFile;
	openInputFile(dataFile, FILENAME);

	int version = readInt(dataFile);
	dataFile.close();

	cout << "Current data file version: v" << version << "\n";

	return version;
}

void readData(int& num1, int& num2)
{
	ifstream dataFile;
	openInputFile(dataFile, FILENAME);

	// Read the version number and discard it
	int tmp = num1 = readInt(dataFile);

	// Read the two data values
	num1 = readInt(dataFile);
	num2 = readInt(dataFile);

	dataFile.close();
}

int main()
{

	cout << "Update server\n";

	int localVersion = getLocalVersion();
	readData(num1, num2);

	//cin >> PORT;
	//cin.ignore();

	// Loads Windows DLL (Winsock version 2.2) used in network programming
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		cerr << "ERROR: Problem with WSAStartup\n";
		return 1;
	}

	// Create a new socket to listen for client connections
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (listenSocket == INVALID_SOCKET)
	{
		cerr << "ERROR: Cannot create socket\n";
		WSACleanup();
		return 1;
	}

	// Setup a SOCKADDR_IN structure which will be used to hold address
	// and port information. Notice that the port must be converted
	// from host byte order to network byte order.
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

	//serverAddr.sin_addr.s_addr = inet_addr( "127.0.0.1" );   <== deprecated

	

	// Attempt to bind to the port.
	if (bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		cerr << "ERROR: Cannot bind to port\n";
		cleanup(listenSocket);
		return 1;
	}

	cout << "Running on port number " << PORT << "\n";

	while (true)
	{
	// Start listening for incoming connections
	if (listen(listenSocket, 1) == SOCKET_ERROR)
	{
		cerr << "ERROR: Problem with listening on socket\n";
		return 1;
	}

	cout << "\nWaiting for connections...\n";

	// Accept incoming connection.  Program pauses here until
	// a connection arrives.
	acceptSocket = accept(listenSocket, NULL, NULL);

	// For this program, the listen socket is no longer needed so it will be closed
	//closesocket(listenSocket);

	cout << "Connection received\n";

	bool done = false;
	char		sendMessage[STRLEN];
	char		recvMessage[STRLEN];


	
		// Wait to receive a message from the remote computer
		//cout << "\n\t--WAIT--\n\n";
		int iRecv = recv(acceptSocket, recvMessage, STRLEN, 0);
		if (iRecv > 0)
		{
			if (strcmp(recvMessage, "1") == 0) { //Request for server version
				cout << "\tRequest for current version number: v" << localVersion << "\n";

				//Send server version to client
				int iSend = send(acceptSocket, (char*)&localVersion, sizeof(localVersion), 0);
				if (iSend == SOCKET_ERROR)
				{
					cerr << "ERROR: Failed to send message\n";
					cleanup(mySocket);
					return 1;
				}
			}
			else if (strcmp(recvMessage, "2") == 0) {//Request for server data
				cout << "\tRequest for update: v" << localVersion << "\n";
				//SEND 1st number
				int iSend = send(acceptSocket, (char*)&num1, sizeof(num1) + 1, 0);
				if (iSend == SOCKET_ERROR)
				{
					cerr << "ERROR: Failed to send message\n";
					cleanup(mySocket);
					return 1;
				}
				//SEND 2nd number
				int iSend2 = send(acceptSocket, (char*)&num2, sizeof(num2) + 1, 0);
				if (iSend2 == SOCKET_ERROR)
				{
					cerr << "ERROR: Failed to send message\n";
					cleanup(mySocket);
					return 1;
				}
			}
			else {
				int iSend = send(acceptSocket, "Did not understand request.", strlen("Did not understand request.") + 1, 0);
				if (iSend == SOCKET_ERROR)
				{
					cerr << "ERROR: Failed to send message\n";
					cleanup(mySocket);
					return 1;
				}
			}
			closesocket(acceptSocket);
			cout << "\tConnection closed\n";
			//WSACleanup();
		}
		else if (iRecv == 0)
		{
			cout << "\tConnection closed\n";
			cleanup(acceptSocket);
			return 0;
		}
		else
		{
			cerr << "ERROR: Failed to receive message\n";
			cleanup(acceptSocket);
			return 1;
		}
		closesocket(mySocket);
	
		requestCount++;
		requestSinceUpdate++;
		cout << "Total requests handled: " << requestCount << "\n";
		if (requestSinceUpdate >= HOTSWAP) {
			cout << "Hot swap updating...\n";
			localVersion = getLocalVersion();
			readData(num1, num2);
			requestSinceUpdate = 0;
		}

	}

	//std::cout << "Paused. Press Enter to continue.";
	//std::cin.ignore(100000, '\n');

	cleanup(acceptSocket);

	return 0;
}

