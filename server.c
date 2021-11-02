#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>

int main(int argc, char * argv[]) {
  if (argc < 3) { 
    printf("Comandos insuficientes");
    exit(1);
  }
  char * address = argv[1];
  int port;
  sscanf(argv[2], "%d", &port);

  int listenfd = 0, connfd = 0;
  struct sockaddr_in serv_addr;

  char sendBuff[1025];
  time_t ticks;

  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  
  memset(&serv_addr, '0', sizeof(serv_addr));
  memset(sendBuff, '0', sizeof(sendBuff));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(address);
  serv_addr.sin_port = htons(port);

  bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

  listen(listenfd, 10);
  
  printf("Server running on %s:%d...\n", address, port);
  
  while(1) {
    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        
    if(connfd < 0) {
      // controlo error
    }
    
    printf("Connection established on socket %d\n", connfd);
    close(connfd);
  }
}