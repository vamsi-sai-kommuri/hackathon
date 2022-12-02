#include <iostream>
#include <winsock2.h>

#include "network_Can_generated.h" // This was generated by `flatc`.
 
using namespace NetworkModels::CAN; // Specified in the schema.
#include <fstream> // C++ header file for file access


#pragma comment(lib,"ws2_32.lib") 
#pragma warning(disable:4996) 

#define SERVER "127.0.0.1"  // or "localhost" - ip address of UDP server
#define BUFLEN 4096  // max length of answer
#define PORT 8888  // the port on which to listen for incoming data

int main()

{
    system("title UDP Client");

    // initialise winsock
    WSADATA ws;
    printf("Initialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
    {
        printf("Failed. Error Code: %d", WSAGetLastError());
        return 1;
    }
    printf("Initialised.\n");

    // create socket
    sockaddr_in server;
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) // <<< UDP socket
    {
        printf("socket() failed with error code: %d", WSAGetLastError());
        return 2;
    }

    // setup address structure
    memset((char*)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.S_un.S_addr = inet_addr(SERVER);

    std::ifstream infile;
    infile.open("network.can", std::ios::binary | std::ios::in);
    infile.seekg(0,std::ios::end);
    int length = infile.tellg();
    infile.seekg(0,std::ios::beg);
    char data[BUFLEN];
    infile.read(data, length);
    infile.close();

    // send the message
    if (sendto(client_socket, data, BUFLEN, 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR)
    {
        printf("sendto() failed with error code: %d", WSAGetLastError());
        return 3;
    }


    // start communication
    while (true)
    {


        //char answer[BUFLEN] = {};

        char *answer = new char[BUFLEN];

        // try to receive some data, this is a blocking call
        int slen = sizeof(sockaddr_in);
        int answer_length;

        if (answer_length = recvfrom(client_socket, answer, BUFLEN, 0, (sockaddr*)&server, &slen) == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code: %d", WSAGetLastError());
            exit(0);
        }
        // print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
        std::cout << answer << "\n";
        auto metaData = GetRegisterFile(answer);

        auto bufferData = metaData->buffer();

        auto frameData = bufferData->Get(0)->frame();

        auto client_payload = frameData->payload();

        for (size_t i = 0; i < client_payload->size(); i++)
        {
            printf("%d",client_payload->Get(i));
        }

         exit(0);
    }

    closesocket(client_socket);
    WSACleanup();
}