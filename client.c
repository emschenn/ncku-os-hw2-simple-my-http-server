#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void sendRequest(int clientfd,char *query,char *host,char *port)
{
    char request[0xfff];
    sprintf(request,"GET %s HTTP/1.x\r\nHOST: %s:%s\r\n\r\n",query,host,port);
    printf("%s",request);
    send(clientfd,request,strlen(request),0);
}

int main(int argc, char *argv[])
{
    int port = atoi(argv[6]);
    char *host = argv[4];

    //create socket
    int clientfd = 0;
    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        printf("Fail to create a socket.");
    }
    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;

    //localhost test
    info.sin_addr.s_addr = inet_addr(host);
    info.sin_port = htons(port);


    int err = connect(clientfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1) {
        printf("Connection error");
    }


    char response[0xfff];
    sendRequest(clientfd,argv[2],argv[4],argv[6]);
    recv(clientfd,response,sizeof(response),0);
    printf("%s",response);
    close(clientfd);
    return 0;
}
/*
#include "client.h"
#include<stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>

struct addrinfo {
  int             ai_flags;         //input flags
  int             ai_family;         //address family of socket
  int             ai_socktype;       //socket type
  int             ai_protocol;       //ai_protocol
  socklen_t       ai_addrlen;        //length of socket address
  char            *ai_canonname;     //canonical name of service location
  struct sockaddr *ai_addr;          //socket address of socket
  struct addrinfo *ai_next;         // pointer to next in list
};
struct addrinfo *getHostInfo(char* host, char* port) {
  int r;
  struct addrinfo hints, *getaddrinfo_res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  if ((r = getaddrinfo(host, port, &hints, &getaddrinfo_res))) {
    fprintf(stderr, "[getHostInfo:21:getaddrinfo] %s\n", gai_strerror(r));
    return NULL;
  }
  return getaddrinfo_res;
}

int makeConnection(struct addrinfo *info) {
  if (info == NULL) return -1;
  int clientfd;
  for (;info != NULL; info = info->ai_next) {
    if ((clientfd = socket(info->ai_family,
                           info->ai_socktype,
                           info->ai_protocol)) < 0) {
      perror("[establishConnection:35:socket]");
      continue;
    }
	printf("%d\n",connect(clientfd, info->ai_addr, info->ai_addrlen));

    if (connect(clientfd, info->ai_addr, info->ai_addrlen) < 0) {
      close(clientfd);
      perror("[establishConnection:42:connect]");
      continue;
    }
    freeaddrinfo(info);
    return clientfd;
  }
  freeaddrinfo(info);
  return -1;
}

void sendRequest(int clientfd,char *query,char *host,char *port){
	char request[0xfff];
	sprintf(request,"GET %s HTTP/1.x\r\nHOST: %s:%s\r\n\r\n",query,host,port);
	printf("%s",request);
	send(clientfd,request,strlen(request),0);
}

int main(int argc, char *argv[])
{
  	int clientfd;
	clientfd = makeConnection(getHostInfo(argv[4], argv[6]));
	printf("argv-4:%s\n", argv[4]);
	sendRequest(clientfd,argv[2],argv[4],argv[6]);

	char response[0xfff];
	while (recv(clientfd, response, 0xfff, 0) > 0) {
		fputs(response, stdout);
		memset(response, 0, 0xfff);
	}

	close(clientfd);
	return 0;
}*/
