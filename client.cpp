#include <iostream>
#include <winsock2.h>
using namespace std;

#include "network_Can_generated.h" // This was generated by `flatc`.
 
using namespace NetworkModels::CAN; // Specified in the schema.

#pragma comment(lib,"ws2_32.lib") 
#pragma warning(disable:4996) 

#define SERVER "127.0.0.1"  // or "localhost" - ip address of UDP server
#define BUFLEN 512  // max length of answer
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

    /*fLATBUFFER**************************************************************************************************/

    flatbuffers::FlatBufferBuilder builder(1024);
     uint8_t frameId = 0;
     uint8_t payload_Data[] = {0,1,2,3};
     auto payload_vector = builder.CreateVector(payload_Data,4);

     auto payloadLen = 0;
     bool rtr = 0;
     MessageTiming m ;

     MessageTiming v ={m.send_request(),m.arbitration(),m.reception()};


     auto frame1 = CreateFrame(builder,frameId,payload_vector,payloadLen,rtr,FrameType_standard_frame);

     auto meta_frame = CreateMetaFrame(builder,BufferStatus_None,BufferDirection_Tx,CanFDIndicator_can,frame1,&v);


     std::vector<flatbuffers::Offset<MetaFrame>> MetaFrameVector;
     MetaFrameVector.push_back(meta_frame);

    auto metaFrame = builder.CreateVector(MetaFrameVector);
    auto fileR = CreateRegisterFile(builder,metaFrame);

    builder.Finish(fileR);
 
    uint8_t *buf = builder.GetBufferPointer();
    int size = builder.GetSize(); 
    auto metaData = GetRegisterFile(buf);

    // start communication
    while (true)
    {
        char message[BUFLEN];
        printf("Enter message: ");
        cin.getline(message, BUFLEN);

        

        // send the message
        if (sendto(client_socket, message, strlen(message), 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR)
        {
            printf("sendto() failed with error code: %d", WSAGetLastError());
            return 3;
        }

        // receive a reply and print it
        // clear the answer by filling null, it might have previously received data
        char answer[BUFLEN] = {};

        // try to receive some data, this is a blocking call
        int slen = sizeof(sockaddr_in);
        int answer_length;
        if (answer_length = recvfrom(client_socket, answer, BUFLEN, 0, (sockaddr*)&server, &slen) == SOCKET_ERROR)
        {
            printf("recvfrom() failed with error code: %d", WSAGetLastError());
            exit(0);
        }

        cout << answer << "\n";
    }

    closesocket(client_socket);
    WSACleanup();
}