// CS 2690 Program 1
// Simple Windows Sockets Echo Client (IPv6)
// Last update: 2/12/19
// Spencer Miller 601 10/11/24
// Windows 11 VS 2019
//
// Usage: WSEchoClientv6 <server IPv6 address> <server port> <"message to echo">
// Companion server is WSEchoServerv6
// Server usage: WSEchoServerv6 <server port>
//
// This program is coded in conventional C programming style, with the
// exception of the C++ style comments.
//
// I declare that the following source code was written by me or provided
// by the instructor. I understand that copying source code from any other
// source or posting solutions to programming assignments (code) on public
// Internet sites constitutes cheating, and that I will receive a zero
// on this project if I violate this policy.
// ----------------------------------------------------------------------------

#include <stdio.h> // for print functions
#include <stdlib.h> // for exit()
#include <winsock2.h> // for Winsock2 functions
#include <ws2tcpip.h> // adds support for getaddrinfo & getnameinfo for v4+6 name resolution
#include <Ws2ipdef.h> // optional - needed for MS IP Helper
#include <stdbool.h> //include bool

// #define ALL required constants HERE, not inline
// #define is a macro, don't terminate with ';' For example...
#define RCVBUFSIZ 50 // buffer size for received messages
char rcvBuffer[RCVBUFSIZ]; // Buffer to store received data

// declare any functions located in other .c files here

void DisplayFatalErr(char* errMsg)
{
    // Returns errorcode from current task or thread
    fprintf(stderr, "%s: %d\n", errMsg, WSAGetLastError()); 
        WSACleanup();
    exit(1);
}

int main(int argc, char* argv[]) { // argc is # of strings following command, argv[] is array of ptrs to the strings
    // Declare ALL variables and structures for main() HERE, NOT INLINE (including the following...)
    WSADATA wsaData; // contains details about WinSock DLL implementation

  
    //initialize data
    int sock;
    int numArgs;
    numArgs = argc;

    //Server IP address and port number come from the client command line
    char* ip;
    int port;
    const char* message;
    int msgLen;
    int fromSize;

    if (numArgs != 4) {
        printf("Number of arguments are not corret... Must have 4.");
        exit(1);
    }

    //get ip and port from command line
    ip = argv[1];
    port = (unsigned short)atoi(argv[2]);
    message = argv[3];
    msgLen = strlen(message) + 1;//account for null terminator


    //Declare structure and variables
    struct sockaddr_in6 serverInfo;
    //procedural code
    memset(&serverInfo, 0, sizeof(serverInfo));//zero out the structure

    //load server info into sockadd_in6
    serverInfo.sin6_family = AF_INET6;
    serverInfo.sin6_port = htons(port); //convert int port into network order*

    //convert server addr from char to ntwrk form, load into socketaddr_in6
    if (inet_pton(AF_INET6, ip, &serverInfo.sin6_addr) <= 0) {
        int err = WSAGetLastError(); // Get the error code
        fprintf(stderr, "inet_pton() failed to convert IP address: %s. Error: %d\n", ip, err);
        exit(1); // Exit with error code
    }

    //Initialize Winsock 2.0 DLL
    int result = WSAStartup(MAKEWORD(2, 0), &wsaData);
    if (result != 0) {
        fprintf(stderr, "Failed to initialize Winsock");
        exit(1);
    }
    //set UDP socket
    sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    //check for success of socket
    if (sock == INVALID_SOCKET) {
        DisplayFatalErr("socket() function failed for sock1.");
    }
    else {
        printf("Socket created successfully. Press ENTER key to continue...");
        getchar();
    }

    //Check for null msg(length = 0)
    if (msgLen == 0) {
        DisplayFatalErr("Cannot have an empty message!\n");
    }

    //send message to server
    if (sendto(sock, message, msgLen, 0, (struct sockaddr *) &serverInfo, sizeof(serverInfo)) != msgLen) {
        DisplayFatalErr("Failed to Send Message\n");
    }
    else {
        printf("Sent Successfull!\n");
    }

    //receive function
    int PASCAL FAR recv(SOCKET s, char FAR * buf, int len, int flags);
    int bytesRead;
    int totalBytesReceived = 0;//should equal message len
    int PASCAL FAR closesocket(SOCKET s);// closing socket when finished

    printf("msg len(expected bytes to receive): %d\n", msgLen);//expected bytes to receive
    //same address received from should be same address sent to.
    struct sockaddr_in6 fromAddr;
    fromSize = sizeof(fromAddr);
    while ((bytesRead = recvfrom(sock, rcvBuffer, RCVBUFSIZ - 1, 0, (struct sockaddr*)&fromAddr, &fromSize)) != SOCKET_ERROR){
        //errors
        if (bytesRead <= 0) {
            DisplayFatalErr("Message not received.");
        }
        if (memcmp(&fromAddr.sin6_addr, &serverInfo.sin6_addr, sizeof(fromAddr.sin6_addr)) != 0) {
            DisplayFatalErr("Message received from unknown server.");
        }
        if (bytesRead != msgLen) {
            DisplayFatalErr("Message length received doesn't match original message length.");
        }

        totalBytesReceived += bytesRead;//updating total bytes
        rcvBuffer[bytesRead] = '\0';//C string for printing


        printf("Received chunk: %s\n", rcvBuffer); // Print received chunk

        //all the message has been received --> exit
        if (totalBytesReceived == msgLen) {
            printf("Message has been fully received!\n");
            break;
        }
    }

    closesocket(sock);//close socket and send FIN
    printf("Socket Closed Successfully...\n");
    WSACleanup();//cleanup
    printf("Winsock resources cleaned up successfully...\n");


    exit(0);
}


//DONE:

/*
1. Verify the correct number of command line arguments have been provided by the user on the
client and server command lines. You don’t need to validate the content of those arguments.
Use the chosen default port if desired


*/