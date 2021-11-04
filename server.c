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
#include <sys/sendfile.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "threadpool.h"

typedef struct request_attributes t_request_attributes;
struct request_attributes {
	int connfd;						
};

void *handleRequest (void *params) {
  t_request_attributes *attributes = (t_request_attributes*) params;
  printf("Conexion establecida en socket %d\n", attributes->connfd);

  const char *fromfile = "image.png";
  struct stat stat_buf;
  int fromfd = open(fromfile, O_RDONLY);
  fstat(fromfd, &stat_buf);
  char response[20048];
  snprintf(response, sizeof(response),
      "HTTP/1.1 200 OK\nContent-Type: %s\nContent-Length: %ld\n\n", "image/png", stat_buf.st_size);
  send(attributes->connfd, response, strlen(response), 0);
  sendfile(attributes->connfd, fromfd, 0, stat_buf.st_size);
  close(attributes->connfd);
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
  
  threadpool_t *pool = threadpool_create(5, 100, 0);

  printf("Server running on %s:%d...\n", address, port);
  
  while(1) {
    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        
    if(connfd < 0) {
      printf("Error al aceptar conexion");
      exit (1);
    }
    t_request_attributes* cxAttributes = malloc(sizeof(t_request_attributes));
    cxAttributes->connfd = connfd;

    threadpool_add(pool, &handleRequest, cxAttributes, 0);
  }

  threadpool_destroy(pool, 0);
  return 0;
}