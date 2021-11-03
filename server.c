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

typedef struct request_attributes t_request_attributes;
struct request_attributes {
	int connfd;						
};

void *handleRequest (void *params) {
  t_request_attributes *attributes = (t_request_attributes*) params;
  printf("Conexion establecida en socket %d\n", attributes->connfd);

  FILE *pictureFile;
  pictureFile = fopen("image.png", "r");
  char * fbuffer = 0;
  long length;

  fseek (pictureFile, 0, SEEK_END);
  length = ftell(pictureFile);
  fseek (pictureFile, 0, SEEK_SET);
  fbuffer = malloc(length);
  int total = 0;
  while (total != length) {
    total += fread(fbuffer, 1, length, pictureFile);
  }
  char response[20048];
  int hlen;

  hlen = snprintf(response, sizeof(response),
      "HTTP/1.1 200 OK\nContent-Type: %s\nContent-Length: %d\n\n", "image/png", length);
  memcpy(response + hlen, fbuffer, length);

  send(attributes->connfd, response, hlen + length, 0);
  close(attributes->connfd);
	pthread_exit ((void *)"Cerrando hilo");
}

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
      printf("Error al aceptar conexion");
      exit (1);
    }
    pthread_t thread;
    pthread_attr_t 	attributes;

    pthread_attr_init (&attributes);
    pthread_attr_setdetachstate (&attributes, PTHREAD_CREATE_JOINABLE);

    t_request_attributes* cxAttributes = malloc(sizeof(t_request_attributes));
    cxAttributes->connfd = connfd;

    pthread_create(&thread, &attributes, handleRequest, cxAttributes);
  }

  return 0;
}