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

  int sock = 0;
  struct sockaddr_in serv_addr;
  char *message = "Hola";
  sock = socket(AF_INET, SOCK_STREAM, 0);
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = inet_addr(address);
  
  connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  send(sock, message, strlen(message), 0 );
  printf("Mensaje enviado\n");
  return 0;
}