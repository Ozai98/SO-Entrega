#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <unistd.h>
#define BACKLOG 2
#define PORT 3535
#define MESSAGESIZE 22

// struct sockadd_in{
//   short sin_family; //  AF_INET
//   unsigned short sin_port;
//   struct in_addr sin_addr;
//   char sin_zero[8];
// };
// struct in_addr{
//   unsigned long s_addr;
// }

// Endianismo
//  big endian
//  little endian
// htons(3535);

int main(){
  int serversd, clientsd[32];
  struct sockaddr_in server, client[32];
  socklen_t clientSize;
  int serverSize, r;

  serversd = socket(AF_INET, SOCK_STREAM, 0);
  if(serversd == -1){
    perror("Socket error\n");
    exit(-1);
  }

  // Assigning a name to a socket
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);
  server.sin_addr.s_addr = INADDR_ANY;
  bzero(server.sin_zero, 8);

  serverSize = sizeof(struct sockaddr);

  r = bind(serversd, (struct sockaddr*)&server, serverSize);
  if(r == -1){
    perror("Bind error\n");
    exit(-1);
  }

  r = listen(serversd, BACKLOG);
  if(r == -1){
    perror("Listen error\n");
    exit(-1);
  }
 r = accept(serverSocket, (struct sockaddr*)&client, (socklen_t*)&size);

  // process or thread
  clientsd[0] = accept(serversd, (struct sockaddr*)&client[0], (socklen_t*)&clientSize);
  if(clientsd[0] == -1){
    perror("Accept error\n");
    exit(-1);
  }
  int *option= 0;
  r = recv(clientsd[0], option, sizeof(int), 0);
  if(r != sizeof(int)){
    perror("Recv error");
    exit(-1);
  }

  printf("option: %i\n", *option);

  // r = send(clientsd, message, MESSAGESIZE, 0);
  // if(r == -1){
  //   perror("Send error\n");
  //   exit(-1);
  // }



  close(clientsd[0]);
  close(serversd);
}
