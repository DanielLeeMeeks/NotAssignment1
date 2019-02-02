#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <iostream>
#include "FileHelper.h"
#include <string>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

const int STRLEN = 256;

WSADATA		wsaData;
SOCKET		mySocket;
SOCKADDR_IN	serverAddr;
const char FILENAME[] = "data.bin";
const char IPADDR[] = "127.0.0.1";
const int  PORT = 50000;
const int  QUERY = 1;
const int  UPDATE = 2;
char		recvMessage[STRLEN];
char		sendMessage[STRLEN];

void cleanup(SOCKET socket)
{
	closesocket(socket);
	WSACleanup();
}

// Returns the version number from the data file
int getLocalVersion();

// Reads the two data values from the data file.
// When the function ends, num1 and num2 will be holding the
// two values that were read from the file.
void readData(int& num1, int& num2);


int main()
{
	int			sum;
	int			num1 = 0;
	int			num2 = 0;
	int			localVersion = 0;
	int			currentVersion = 0;

	// Add code here to
	// 1) make sure that we are using the current version of the data file
	// 2) update the data file if it is out of data

	// Main purpose of the program starts here: read two numbers from the data file and calculate the sum
	localVersion = getLocalVersion();

	// Loads Windows DLL (Winsock version 2.2) used in network programming
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		cerr << "ERROR: Problem with WSAStartup\n";
		return 1;
	}

	// Create a new socket for communication
	mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (mySocket == INVALID_SOCKET)
	{
		cerr << "ERROR: Cannot create socket\n";
		WSACleanup();
		return 1;
	}

	//cout << "\nAttempting to connect...\n";

	// Setup a SOCKADDR_IN structure which will be used to hold address
	// and port information for the server. Notice that the port must be converted
	// from host byte order to network byte order.
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, IPADDR, &serverAddr.sin_addr);

	//serverAddr.sin_addr.s_addr = inet_addr( ipAddress );   <== deprecated

	// Try to connect
	if (connect(mySocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		cerr << "ERROR: Failed to connect\n";
		cleanup(mySocket);
		return 1;
	}

	cout << "Checking for updates...\n";

	//Send request for version number
	int iSend = send(mySocket, "1", strlen("1") + 1, 0);
	if (iSend == SOCKET_ERROR)
	{
		cerr << "ERROR: Failed to send message\n";
		cleanup(mySocket);
		return 1;
	}

	// Wait to receive a reply message back from the remote computer
	//cout << "\n\t--WAIT--\n\n";
	int iRecv = recv(mySocket, recvMessage, STRLEN, 0);
	if (iRecv > 0)
	{
		//change version number char[4] into int
		std::memcpy(&currentVersion, recvMessage, sizeof(int));
		//cout << "Recv  -- > " << currentVersion << "\n";
	}
	else if (iRecv == 0)
	{
		//cout << "Connection closed\n";
		cleanup(mySocket);
		return 0;
	}
	else
	{
		cerr << "ERROR: Failed to receive message\n";
		cleanup(mySocket);
		return 1;
	}

	//cout << localVersion << " CUR-> " << currentVersion << "\n";
	
	//Update local file
	if (localVersion != currentVersion) {
		
		cout << "Downloading updates...\n";

		//ofstream dataFile;
		//openOutputFile(dataFile, FILENAME);
		//writeNotInt(dataFile, recvMessage);

			mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (mySocket == INVALID_SOCKET)
			{
				cerr << "ERROR: Cannot create socket\n";
					WSACleanup();
					return 1;
			}

		//cout << "\nAttempting to connect...\n";

		// Setup a SOCKADDR_IN structure which will be used to hold address
		// and port information for the server. Notice that the port must be converted
		// from host byte order to network byte order.
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(PORT);
		inet_pton(AF_INET, IPADDR, &serverAddr.sin_addr);

		//serverAddr.sin_addr.s_addr = inet_addr( ipAddress );   <== deprecated

		// Try to connect
		if (connect(mySocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			cerr << "ERROR: Failed to connect\n";
			cleanup(mySocket);
			return 1;
		}

		//cout << "Connected...\n\n";

		//Request update data
		int iSend = send(mySocket, "2", strlen("2") + 1, 0);

		// Wait to receive a reply message back from the server with num1
		//cout << "\n\t--WAITING FOR NUM1--\n\n";
		int iRecv = recv(mySocket, recvMessage, STRLEN, 0);
		if (iRecv > 0)
		{
			//Change num1 char[4] to int
			std::memcpy(&num1, recvMessage, sizeof(int));
			//cout << "Recv num 1 > " << num1 << "\n";
		}
		else if (iRecv == 0)
		{
			cout << "Connection closed\n";
			cleanup(mySocket);
			return 0;
		}
		else
		{
			cerr << "ERROR: Failed to receive message\n";
			cleanup(mySocket);
			return 1;
		}
		// Wait to receive a reply message back from the server with num2
		//cout << "\n\t--WAITING FOR NUM2--\n\n";
		int iRecv2 = recv(mySocket, recvMessage, STRLEN, 0);
		if (iRecv2 > 0)
		{
			//Change num2 char[4] to int
			std::memcpy(&num2, recvMessage, sizeof(int));
			//cout << "Recv num 2 > " << num2 << "\n";
		}
		else if (iRecv2 == 0)
		{
			cout << "Connection closed\n";
			cleanup(mySocket);
			return 0;
		}
		else
		{
			cerr << "ERROR: Failed to receive message\n";
			cleanup(mySocket);
			return 1;
		}

		
		//Write new numbers to file
		ofstream dataFile;
		openOutputFile(dataFile, FILENAME);
		writeInt(dataFile, currentVersion);
		writeInt(dataFile, num1);
		writeInt(dataFile, num2);
		cout << "Update finished\n";
	
	}
	else {
		cout << "No updates found\n";
	}
	closesocket(mySocket);
	WSACleanup();

	//Start of Calculator program
	cout << "\nSum Calculator Version " << localVersion << "\n";

	//Read numbers from local file.
	readData(num1, num2);	
	sum = num1 + num2;
	cout << "\nThe sum of " << num1 << " and " << num2 << " is " << sum << "\n" << endl;

	//Wait until user presses enter to close cmd.
	//std::cout << "Paused. Press Enter to continue.";
	//std::cin.ignore(100000, '\n');

	return 0;
}

// Returns the version number from the data file
int getLocalVersion()
{
	ifstream dataFile;
	openInputFile(dataFile, FILENAME);

	int version = readInt(dataFile);
	dataFile.close();

	return version;
}

// Reads the two data values from the data file.
// When the function ends, num1 and num2 will be holding the
// two values that were read from the file.
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