#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
char response[0xfff];

void substr(char *dest,const char *src,int start,int cnt)
{
    strncpy(dest,src+start,cnt);
    dest[cnt]=0;
}
void listFile(char* root)
{
    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(root))!=NULL) {
        while((ent = readdir(dir))!=NULL) {
            printf("%s\n",ent->d_name);
        }
        closedir(dir);
    } else {
        perror("");
        return EXIT_FAILURE;
    }
}

char* getType(char *queryType)
{
    if(strcmp(queryType,"htm")==0)
        return "text/html";
    else if(strcmp(queryType,"html")==0)
        return "text/html";
    else if(strcmp(queryType,"css")==0)
        return "text/css";
    else if(strcmp(queryType,"h")==0)
        return "text/x-h";
    else if(strcmp(queryType,"hh")==0)
        return "text/x-h";
    else if(strcmp(queryType,"c")==0)
        return "text/x-c";
    else if(strcmp(queryType,"cc")==0)
        return "text/x-c";
    else if(strcmp(queryType,"json")==0)
        return "application/json";
    else
        return NULL;
}

void* respond(char *msg,char *root)
{
    char status[0xff];
    char *type;
    char content[0xfff] = "";
    char *method = strtok(msg," ");
    char *query = strtok(NULL," ");

    char judge="GET";
    char file[0xff];
    memset(file,'\0',sizeof(file));
    strcpy(file,root);
    strcat(file,query);

    char *q = strtok(query,".");
    char *queryType = strtok(NULL,".");
    if(queryType==NULL) type = "directory";
    else    type = getType(queryType);


    if(query[0]!='/')
        strcpy(status,"400 Bad Request");
    //else if(strcmp(method,judge)!=0)
    //  strcpy(status,"405 Method Not Allowed");
    else if(type==NULL)
        strcpy(status,"415 Unsupported Media Type");
    else if(access(file,F_OK)!=0)
        strcpy(status,"404 Not Found");
    else
        strcpy(status,"200 OK");
    sprintf(response,"HTTP/1.x %s\r\nContent-Type: %s\r\nServer: httpserver/1.x\r\n\r\n%s",status,type,content);
}
int main(int argc, char *argv[])
{

    //  char *root = argv[2];
    listFile(argv[2]);
    int port = atoi(argv[4]);
    int THREAD_NUM = atoi(argv[6]);
    //create socket
    char inputBuffer[256] = {};
    char message[] = {"Hi,this is server.\n"};
    int serverfd = 0,forClientSockfd = 0;
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1) {
        printf("Fail to create a socket.");
    }

    struct sockaddr_in serverInfo,clientInfo;
    int addrlen = sizeof(clientInfo);
    bzero(&serverInfo,sizeof(serverInfo));

    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_port = htons(port);
    bind(serverfd,(struct sockaddr *)&serverInfo,sizeof(serverInfo));
    listen(serverfd,5);


    //receive
    while(1) {
        forClientSockfd = accept(serverfd,(struct sockaddr*) &clientInfo, &addrlen);


        recv(forClientSockfd,inputBuffer,sizeof(inputBuffer),0);
        //printf("Get:%s\n",inputBuffer);
        respond(inputBuffer,argv[2]);
        send(forClientSockfd,response,sizeof(response),0);

    }
    return 0;
}
/*
#include "server.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <malloc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <signal.h>
#define _XOPEN_SOURCE 600
#define BYTES 1024
int listenfd, clients[10];

pthread_mutex_t lock;
pthread_cond_t emp;
int sock =0;
int max_threads =0;
int global_count=0;
int finished =1;
int length=0;


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

extn extensions[] = {
    {"htm","text/html"},
    {"html","text/html"},
    {"css","text/css"},
    {"h","text/x-h"},
    {"hh","text/x-h"},
    {"c","text/x-c"},
    {"cc","text/x-c"},
    {"json","application/json"},
    {0,0}
};

enum{
    OK = 0,
    BAD_REQUEST,
    NOT_FOUND,
    METHOD_NOT_ALLOWED,
    UNSUPPORT_MEDIA_TYPE
};

const int status_code[] = {
    200,
    400,
    404,
    405,
    415
};

int startServer(char *port){
    struct addrinfo hints, *res;
    memset (&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo("127.0.0.1", port, &hints, &res) != 0){
        perror("error");
        exit(1);
    }
    int serverfd = socket(AF_INET,SOCK_STREAM,0);                  res->ai_socktype,

   // if (res == NULL) serverfd = -1;

    for (;res != NULL; res = res->ai_next) {
        if ((serverfd = socket(res->ai_family,
                            res->ai_socktype,
                            res->ai_protocol)) < 0) {
            perror("[bindListener:35:socket]");
            continue;
        }
        int opt = 1;
        if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR,
                    &opt, sizeof(int)) < 0) {
            perror("[bindListener:43:setsockopt]");
            return -1;
        }
        if (bind(serverfd, res->ai_addr, res->ai_addrlen) < 0) {
            close(serverfd);
            perror("[bindListener:49:bind]");
            continue;
        }
        freeaddrinfo(res);
        return serverfd;
    }
    freeaddrinfo(res);
    return -1;
}


//client connection
void respond(int n,char *root)
{
	char response[0xfff], *reqline[3], data_to_send[BYTES], path[99999];
	int rcvd, fd, bytes_read;

	//memset( (void*)me, (int)'\0', 99999 );

	rcvd=recv(clients[n], response, 0xfff, 0);

	if (rcvd<0)    // receive error
		fprintf(stderr,("recv() error\n"));
	else if (rcvd==0)    // receive socket closed
		fprintf(stderr,"Client disconnected upexpectedly.\n");
	else    // message received
	{
		printf("%s", response);
		reqline[0] = strtok (response, " \t\n");
		if ( strncmp(reqline[0], "GET\0", 4)==0 )
		{
			reqline[1] = strtok (NULL, " \t");
			reqline[2] = strtok (NULL, " \t\n");
			if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
			{
				write(clients[n], "HTTP/1.0 400 Bad Request\n", 25);
			}
			else
			{
				if ( strncmp(reqline[1], "/\0", 2)==0 )
					reqline[1] = "/index.html";        //Because if no file is specified, index.html will be opened by default (like it happens in APACHE...

				strcpy(path, root);
				strcpy(&path[strlen(root)], reqline[1]);
				printf("file: %s\n", path);

				if ( (fd=open(path, O_RDONLY))!=-1 )    //FILE FOUND
				{
					send(clients[n], "HTTP/1.0 200 OK\n\n", 17, 0);
					while ( (bytes_read=read(fd, data_to_send, BYTES))>0 )
						write (clients[n], data_to_send, bytes_read);
				}
				else    write(clients[n], "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
			}
		}
	}

	//Closing SOCKET
	shutdown (clients[n], SHUT_RDWR);         //All further send and recieve operations are DISABLED...
	close(clients[n]);
	clients[n]=-1;
}

int main(int argc, char *argv[])
{
    //printf("\n%s",getenv("PWD"));
    int THREAD_NUM = atoi(argv[6]);
    int i,s = 0;
    int servfd;
    //printf("%s %s %d",root,port,THREAD_NUM);
    int server = startServer(argv[4]);
    struct sockaddr_in clientaddr;
	socklen_t addrlen;
    printf("a");
    while(1){
        addrlen = sizeof(clientaddr);
		clients[s] = accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);
        if(clients[s]<0)
            error("accept error");
        else{
			//	resolve(clients[s]);
            printf("b");
            close(clients[s]);
        }
        while(clients[s] != -1) s++;
    }
    //thread pool
    pthread_t t[THREAD_NUM];
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    for(i = 0; i < THREAD_NUM; i++){
     //   pthread_create(&t[i], &attr,NULL,dequeue, (void*) &i);
    }
    close(server);

    //socket

    // create socket
    servfd = socket(AF_INET, SOCK_STREAM, 0);
    int sock = servfd;
    if (servfd == -1) {
        printf("Error creating socket\n");
        exit(1);
    }

    // init prepernces
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    // bind
    setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, (const char *) &len_of_packet,sizeof(int));
    if (bind(servfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
        printf("Error: Bind has been failed\n");
        perror ("Bind: ");
        exit(1);
    }

    //listen
    if (listen(servfd, requests) == -1) {
        printf("Error: Listen has been failed\n");
        perror ("Listen: ");
        exit(1);
    }


}*/

