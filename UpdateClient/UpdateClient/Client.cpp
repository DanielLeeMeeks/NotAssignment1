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

	cout << "\nAttempting to connect...\n";

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
		//cleanup(mySocket);
		return 1;
	}

	cout << "Connected...\n\n";

	int iSend = send(mySocket, "1", strlen("1") + 1, 0);
	if (iSend == SOCKET_ERROR)
	{
		cerr << "ERROR: Failed to send message\n";
		//cleanup(mySocket);
		return 1;
	}

	// Wait to receive a reply message back from the remote computer
	cout << "\n\t--WAIT--\n\n";
	int iRecv = recv(mySocket, recvMessage, STRLEN, 0);
	if (iRecv > 0)
	{
		cout << "Recv > " << recvMessage << "\n";
	}
	else if (iRecv == 0)
	{
		cout << "Connection closed\n";
		//cleanup(mySocket);
		return 0;
	}
	else
	{
		cerr << "ERROR: Failed to receive message\n";
		//cleanup(mySocket);
		return 1;
	}

	if (localVersion != currentVersion) {
		//UPDATE
		cout << "\nNot up to date.";

			mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (mySocket == INVALID_SOCKET)
			{
				cerr << "ERROR: Cannot create socket\n";
					WSACleanup();
					return 1;
			}

		cout << "\nAttempting to connect...\n";

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
			//cleanup(mySocket);
			return 1;
		}

		cout << "Connected...\n\n";

		int iSend = send(mySocket, "2", strlen("2") + 1, 0);

		// Wait to receive a reply message back from the remote computer
		cout << "\n\t--WAIT--\n\n";
		int iRecv = recv(mySocket, recvMessage, STRLEN, 0);
		if (iRecv > 0)
		{
			cout << "Recv > " << recvMessage << "\n";
		}
		else if (iRecv == 0)
		{
			cout << "Connection closed\n";
			//cleanup(mySocket);
			return 0;
		}
		else
		{
			cerr << "ERROR: Failed to receive message\n";
			//cleanup(mySocket);
			return 1;
		}

		
	}
	closesocket(mySocket);
	WSACleanup();

	cout << "\nSum Calculator Version " << localVersion << "\n\n";

	readData(num1, num2);	
	sum = num1 + num2;
	cout << "The sum of " << num1 << " and " << num2 << " is " << sum << endl;

	std::cout << "Paused. Press Enter to continue.";
	std::cin.ignore(100000, '\n');

	return 0;
}

int getLocalVersion()
{
	ifstream dataFile;
	openInputFile(dataFile, FILENAME);

	int version = readInt(dataFile);
	dataFile.close();

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

void cleanup(SOCKET socket)
{
	closesocket(socket);
	WSACleanup();
}