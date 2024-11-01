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
#define MAXECHO 512
const char rcvBuffer[RCVBUFSIZ]; // Buffer to store received data

// declare any functions located in other .c files here
#ifndef DISPLAYFATALERR_H
#define DISPLAYFATALERR_H
void DisplayFatalErr(const char* errMsg);
#endif // !DISPLAYFATALERR_H


int main(int argc, char* argv[]) {
    // Declare ALL variables and structures for main() HERE, NOT INLINE (including the following...)
    WSADATA wsaData; // contains details about WinSock DLL implementation
    //initialize data
    int numArgs = argc;
    //int MAXQUEUED = 128; // maximum simultaneous client connection requests.
    int port;
    int serverSock;
    struct sockaddr_in6 clientInfo; //Holds client port & addr after client connects
    int clientInfoLen;
    int clientSock;
    char echoBuffer[MAXECHO];
    int echoLen;
    
    //verify the correct number of command line arguments have been provided by the user.
    if (numArgs != 2) {
        printf("Number of arguments are not corret... Must have 2.");
        exit(1);
    }
    //default port
    port = 2000;
    //if port is input correctly
    if ((unsigned short)atoi(argv[1])) {
        port = (unsigned short)atoi(argv[1]);
    }

    //initialize Winsock
    if ((WSAStartup(MAKEWORD(2, 0), &wsaData)) != 0) {
        DisplayFatalErr("Winsock Unsuccessfully Loaded");
    }

    serverSock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP); // Create server socket

    //server info
    struct sockaddr_in6 serverInfo;
    memset(&serverInfo, 0, sizeof(serverInfo)); // zero out the structure

    //load server info into sockadd_in6
    serverInfo.sin6_family = AF_INET6;
    serverInfo.sin6_port = htons(port); //convert int port into network order*
    serverInfo.sin6_addr = in6addr_any; //Use in6addr_any as the server’s IP address

    //bind() the server socket to this sockaddr_in6 structure.
    if (bind(serverSock, (struct sockaddr*)&serverInfo, sizeof(serverInfo)) != 0) {
        DisplayFatalErr("Bind failed");
    }


    printf("SRM's IPv6 echo server is ready for client connection...\n");
    //getchar();

    clientInfoLen = sizeof(clientInfo);
    for (;;) {
        echoLen = recvfrom(serverSock, echoBuffer, MAXECHO, 0, (struct sockaddr*)&clientInfo, &clientInfoLen);

        if (echoLen == SOCKET_ERROR) {
            DisplayFatalErr("recvfrom error");
        }

        printf("Echo Length! %d\n", echoLen);
        //display the IP address and port number of the client, and the server’s own port number
        char clientIP[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &clientInfo.sin6_addr, clientIP, sizeof(clientIP));
        unsigned short clientPort = ntohs(clientInfo.sin6_port);
        printf("Processing the client at %s, client port %d, server port %d\n", clientIP, clientPort, port);

        if (sendto(serverSock, echoBuffer, echoLen, 0, (struct sockaddr*)&clientInfo, clientInfoLen) == SOCKET_ERROR) {
            DisplayFatalErr("sendto error!");
        }
        else {
            printf("sent successfully");
        }


    }
    exit(0);
}

//DONE:

/*
1. Verify the correct number of command line arguments have been provided by the user on the
client and server command lines. You don’t need to validate the content of those arguments.
Use the chosen default port if desired

2. Display a message on the server console that includes your initials, similar to this:
JD's IPv6 echo server is ready for UDP client...


*/