#include <iostream>
#include <winsock2.h>
#include "../inc/network_Can_generated.h" // This was generated by `flatc`.
#include <fstream>                  // C++ header file for file access

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable : 4996)

#define SERVER "127.0.0.1" // or "localhost" - ip address of UDP server
#define BUFLEN 4096        // max length of answer
#define PORT 8888          // the port on which to listen for incoming data
using namespace NetworkModels::CAN; // Specified in the schema.

sockaddr_in server;
int client_socket;

int formConnection() {
  flatbuffers::FlatBufferBuilder builder(1024);
  // initialise winsock
  WSADATA ws;
  int frameId = 12;
  uint8_t payload_Data[] = {0, 1, 2, 3};
  auto payload_vector = builder.CreateVector(payload_Data, 4);
  auto payloadLen = size_t(payload_Data);
  bool rtr = 0;
  MessageTiming m;
  system("title UDP Client");
  printf("Initialising Winsock...");

  if (WSAStartup(MAKEWORD(2, 2), &ws) != 0) {
    printf("Failed. Error Code: %d", WSAGetLastError());
  }
  printf("Initialised.\n");

  if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) ==
      SOCKET_ERROR) // <<< UDP socket
  {
    printf("socket() failed with error code: %d", WSAGetLastError());
  }

  // setup address structure
  memset((char *)&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.S_un.S_addr = inet_addr(SERVER);

  MessageTiming v = {m.send_request(), m.arbitration(), m.reception()};

  auto frame1 = CreateFrame(builder, frameId, payload_vector, payloadLen, rtr,
                            FrameType_extended_fram);
  auto meta_frame =
      CreateMetaFrame(builder, BufferStatus_MAX, BufferDirection_Rx,
                      CanFDIndicator_canFD, frame1, &v);

  std::vector<flatbuffers::Offset<MetaFrame>> MetaFrameVector;
  MetaFrameVector.push_back(meta_frame);
  auto metaFrame = builder.CreateVector(MetaFrameVector);
  auto fileR = CreateRegisterFile(builder, metaFrame);
  builder.Finish(fileR);
  uint8_t *buf = builder.GetBufferPointer();

  // send the message
  if (sendto(client_socket, (char *)buf, BUFLEN, 0, (sockaddr *)&server,
             sizeof(sockaddr_in)) == SOCKET_ERROR) {
    printf("sendto() failed with error code: %d", WSAGetLastError());
  }
  return client_socket;
}
void receiveData(int cli_soc) {

  // start communication
  while (true) {
    char *answer = new char[BUFLEN];
    // try to receive some data, this is a blocking call
    int slen = sizeof(sockaddr_in);
    int answer_length;

    if (answer_length = recvfrom(cli_soc, answer, BUFLEN, 0, (sockaddr *)&server,
                                 &slen) == SOCKET_ERROR) {
      printf("recvfrom() failed with error code: %d", WSAGetLastError());
      exit(0);
    }
    // print details of the client/peer and the data received
    printf("Received packet from %s:%d\n", inet_ntoa(server.sin_addr),
           ntohs(server.sin_port));

    auto metaData = GetRegisterFile(answer);
    auto bufferData = metaData->buffer();
    auto frameData = bufferData->Get(0)->frame();
    auto client_payload = frameData->payload();

    for (size_t i = 0; i < client_payload->size(); i++) {
      printf("%d", client_payload->Get(i));
    }
    exit(0);
  }
  closesocket(client_socket);
  WSACleanup();
}
int main() {
  int client_sock = formConnection();
  receiveData(client_sock);
}