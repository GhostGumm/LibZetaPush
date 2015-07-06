#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
  int serv_sock;
  int clnt_sock;
  int i=0;
  char readHeader[500]={0};
  char* parsingHeader[10];
  char* saveOrigin;
  char* saveKey;
  char* magicKey="258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
  char query[105]="HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: ";

  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size;

  char message[]="Hello World!";

  if(argc!=2){
    printf("Usage : %s <port>\n", argv[0]);
    exit(1);
  }

  serv_sock=socket(PF_INET, SOCK_STREAM, 0);
  if(serv_sock == -1)
    error_handling("socket() error");

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family=AF_INET;
  serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
  serv_addr.sin_port=htons(atoi(argv[1]));

  if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1 )
    error_handling("bind() error");

  if(listen(serv_sock, 5)==-1)
    error_handling("listen() error");

  clnt_addr_size=sizeof(clnt_addr);
  clnt_sock=accept(serv_sock);
  clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr,&clnt_addr_size);
  if(clnt_sock==-1)
    error_handling("accept() error");
  read(clnt_sock,readHeader,355);

  parsingHeader[0]=strtok(readHeader,"\r\n");

  for(i=1;i<6;i++)
    parsingHeader[i]=strtok(NULL,"\r\n");

  saveOrigin=strtok(parsingHeader[4]," ");
  saveOrigin=strtok(NULL," ");
  saveKey=strtok(parsingHeader[5]," ");
  saveKey=strtok(NULL," ");

  strcat(saveKey,magicKey);
  strcat(query,saveKey);
  strcat(query,"\r\n");
  strcat(query,"\r\n");
  printf("%s",query);


  write(clnt_sock, query, sizeof(query));
  close(clnt_sock);
  close(serv_sock);
  return 0;
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}
