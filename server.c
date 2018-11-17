#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <pthread.h>

typedef struct Queue {
    int capacity;
    int size;
    int front;
    int back;
    char** data;
} Queue;
char *root;
Queue *Q = NULL;
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
char response[0xfff];


void createQueue(int maxElements)
{
    Q = (Queue *)malloc(sizeof(Queue));
    Q->data = (char**)malloc(maxElements*sizeof(char*));
    Q->size = 0;
    Q->capacity = maxElements;
    Q->front = 0;
    Q->back = -1;
}
void dequeue(Queue *Q)
{
    if(Q->size==0) {
        printf("Queue is Empty\n");
        return;
    } else {
        Q->size--;
        Q->front++;
        if(Q->front==Q->capacity) {
            Q->front=0;
        }
    }
    return;
}
char* pop(Queue *Q)
{
    if(Q->size==0) {
        printf("Queue is Empty\n");
        exit(0);
    }
    return Q->data[Q->front];
}
void enqueue(Queue *Q,char* msg)
{
    if(Q->size == Q->capacity) {
        printf("Queue is Full\n");
    } else {
        Q->size++;
        Q->back = Q->back + 1;
        if(Q->back == Q->capacity) {
            Q->back = 0;
        }
        Q->data[Q->back] = msg;
    }
    return;
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

void respond(char *msg,char* root)
{
    char status[0xff];
    char *type;
    char content[0xfff] = "";
    char *method = strtok(msg," ");
    char *query = strtok(NULL," ");

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
    //else if(strcmp(method,"GET")!=0)
    //  strcpy(status,"405 Method Not Allowed");
    else if(type==NULL)
        strcpy(status,"415 Unsupported Media Type");
    else if(access(file,F_OK)!=0)
        strcpy(status,"404 Not Found");
    else
        strcpy(status,"200 OK");
    sprintf(response,"HTTP/1.x %s\r\nContent-Type: %s\r\nServer: httpserver/1.x\r\n\r\n%s",status,type,content);
}
/*
void* threadWork(){
    do{
        pthread_mutex_lock(&mutex);
        if (Q->size > 0) {
            char* msg = pop(Q);
            respond(msg);
            dequeue(msg);
        }
        pthread_mutex_unlock(&mutex);
    }while(1);
}
*/
int main(int argc, char *argv[])
{
    root = argv[2];
    printf("%s",root);
    listFile(argv[2]);
    int port = atoi(argv[4]);
    int THREAD_NUM = atoi(argv[6]);
    createQueue(THREAD_NUM);

    //create socket
    char *receive;
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
    /*
    pthread_t t[THREAD_NUM];
    pthread_attr_t attr;
    pthread_attr_init(&attr);/*
    for(int i = 0; i < THREAD_NUM; i++){
        pthread_create(&t[i], NULL,*threadWork,NULL);
    }
    for(int i = 0; i < THREAD_NUM; i++){
        pthread_join(&t[i], NULL);
    }*/
    //receive
    while(1) {
        forClientSockfd = accept(serverfd,(struct sockaddr*) &clientInfo, &addrlen);

        recv(forClientSockfd,receive,sizeof(receive),0);
        enqueue(Q,receive);
        //printf("Get:%s\n",inputBuffer);
        //respond(inputBuffer,argv[2]);


        send(forClientSockfd,response,sizeof(response),0);
    }

    return 0;
}
