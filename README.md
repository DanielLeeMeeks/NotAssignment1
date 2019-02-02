# NotAssignment1
This is not the answers to assignment 1 for C█CI 30██.
[http://rhoggard.cs.ecu.edu/rhoggard/3010/assignments/assg01.aspx](http://rhoggard.cs.ecu.edu/rhoggard/3010/assignments/assg01.aspx)

For this assignment, you will be writing two programs which are designed to talk to each other. One of your programs will be the update server and the other will be the update client. Here is the synopsis of how the two programs interact:

## Requirements
### Server:
- ~~As the server starts up, it reads the version number in the data.bin file and stores it in memory for future use.~~
- ~~The server binds to a port and awaits connections.~~
### Client:
- ~~As the client starts up, it will first read the version number found in it's own data.bin file.~~
- ~~The client contacts the server.~~
### Server:
- ~~Server accepts the incoming connection.~~
- ~~Waits for data to be sent.~~
### Client:
- ~~Send to the server, the integer 1. For the purposes of our program, the integer 1 is a code which represents a request for the current version number.~~
- ~~Wait for a reply from the server.~~
### Server:

- ~~Receive the request.~~
- ~~Check to see what the client is requesting. If the integer 1 was received, then the request is for the version number.
- ~~Send the version number back to the client~~
- ~~Close the current connection and wait for a new connection from a new client.~~
### Client:

- ~~Receive the version number from the server.~~
- ~~Compare the received version number to the client's version number.~~
- ~~If the version numbers match, then the program proceeds with it's normal operation (see below).~~
- ~~If the version numbers do not match:~~
- ~~Contact the server again~~
### Server:

- ~~Server accepts the incoming connection~~
- ~~Waits for data to be sent.~~
### Client:

- ~~Send to the server, the integer 2. For the purposes of our program, the integer 2 is a code which represents a request for the updated file.~~
### Server:

- ~~Receive the request.~~
- ~~Check to see what the client is requesting. If the integer 2 was received, then the request is for the updated file.~~
- ~~Open the data.bin file~~
- ~~Read the bytes from the file~~
- ~~Send the bytes to the client~~
- ~~Close the current connection and wait for a new connection from a new client~~
### Client:

- ~~Receive the bytes from the server.~~
- ~~Open the data.bin file for writing (this action should delete the existing file and then open a new one, ready to write data into).~~
- ~~Write the bytes received into the data.bin file.~~
- ~~Close the file~~
- ~~Proceed with normal operation~~

~~Note that in the above sequence of communication steps, once the server has handled a request (either for the version number or for the updated file), it closes the connection and then goes back to waiting for a new connection.~~

==Also note, although it isn't specifically listed in the steps above, our server will also display results to the screen to show what's happening. Likewise, the client program also displays status messages to let it's user know what's happening.==

## Additional Requirements

The following are a few extra requirements which are done "behind the scenes" and therefore not immediately obvious when executing the demo programs:

-   ~~**Cache Requirement**: The server is required to cache the version number of the data file. This means that the server should only read the version number from the data file once and store it in a variable in memory. It should NOT read the version number from the data file everytime a request is made for the version number. This is done to improve speed, since it is faster to read a value from a variable than it is to read a value from a file.~~
    
-   **Server "Hot Swap" Requirement**: In order to make the data file easier to swap out with new versions, the server does not need to be shut down and restarted. Instead, after every 5 requests handled by the server, it should re-read the version number from the data file. By doing this, the data file can be swapped out between client requests. Ideally, the number "5" should probably be higher in a production server, to benefit more from the cache feature, but we will use 5 for testing purposes.
    
-   ~~**Separate Requests**: If the client's data file needs to be updated, the client is required to make TWO INDEPENDENT REQUESTS to the server.The first request is for the version number, and the second request is for the data file. The second request should send the contents of the entire data file -- including the version number again. Don't assume the client will use the previously sent version number ---- it's possible that it could have changed.~~
