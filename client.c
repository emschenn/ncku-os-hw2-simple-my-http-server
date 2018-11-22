#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <pthread.h>
#include "client.h"

int clientfd = 0,err =0;
char response[50000];
char request[1000];
char *query;
char *port;
char *host;
struct sockaddr_in info;
void sendnrecv(char* query)
{
    int n;
    memset(response,0,sizeof(response));
    memset(request,0,sizeof(request));
    sprintf(request,"GET %s HTTP/1.x\r\nHOST: %s:%s\r\n\r\n",query,host,port);
    //printf("\n\nrepuest:\n%s",request);
    if(write(clientfd,request,sizeof(request))>0) ;
    else    perror("write failed");
    //while((n =
    read(clientfd,response,sizeof(response));
    //memset(response,0,sizeof(response));
    printf("%s\n",response);
}

void process(char* root)
{
    int i=0;
    char *root2;    //char *newRoot;
    root2 = calloc(strlen(root)+1,sizeof(char));
    strcpy(root2,root);
    pthread_t t;
    //char query[30]; strcpy(query,root);
    //printf("\nq:%s\n",response);
    char *response1;    //char *newRoot;
    response1 = calloc(strlen(response)+1,sizeof(char));
    strcpy(response1,response);
    //printf("\nq:%s\n",root);
    char *type = strtok(response1,"\n");
    type = strtok(NULL," ");
    type = strtok(NULL,"\r");
    char *content = strtok(NULL,"\n");
    content = strtok(NULL,"\0");
    //create output
    if(type != NULL) {
        mkdir("output",S_IRWXU|S_IRWXG|S_IROTH);
        if(strcmp(type,"directory")==0) {
            createFolder(root);
        } else {
            createFile(root,content);
        }
    }
    //printf("\nroot:2%s\n",root);
    if((strcmp(type,"directory")==0)) { //for directory
        char *doc[20],newQuery[50];
        doc[i] = strtok(content,"\n ");
        //sprintf(doc[],"%s/%s",query,doc[j]);
        while(doc[i] != NULL) {
            //printf("%d:%s\n",i,doc[i]);
            i++;
            doc[i] = strtok(NULL," ");
        }
        //strcat(root,"/");
        for(int j=1; j < i; j++) {          //judge content
            //printf("%s",doc[j]);
            //printf("%s\n",newQuery);
            if(strchr(doc[j],'.')!=NULL) {  //is file
                sprintf(newQuery,"%s/%s",root2,doc[j]);
                pthread_create(&t,NULL,*threadWork,newQuery);
                pthread_join(t, NULL);
            } else {                        //is folder
                sprintf(newQuery,"%s/%s",root2,doc[j]);
                //strcpy(query,newQuery);
                //printf("\n %d :%s",i,query);
                pthread_create(&t,NULL,*threadWork,newQuery);
                pthread_join(t, NULL);
            }
        }
    }
    free(root2);
    free(response1);
}

void* threadWork(char* newQuery)
{
    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        printf("Fail to create a socket.");
    }
    err = connect(clientfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1) {
        printf("Connection error\n");
    }
    sendnrecv(newQuery);
    //printf("q:%s",newQuery);
    process(newQuery);
}

void createFolder(char* root)
{
    char out_file[100] = {};
    char *out = strtok(root,"/");  //path: output/../..
    sprintf(out_file,"output/%s",out);
    while(out) {
        //printf("\nout:%s\n",out_file);
        mkdir(out_file,S_IRWXU|S_IRWXG|S_IROTH);
        out = strtok(NULL,"/");
        sprintf(out_file+strlen(out_file),"/%s",out);
    }
}

void createFile(char* root,char* content)
{
    char out_file[100] = {};
    char *out = strtok(root,"/");  //path: output/../..
    strcat(out_file,"output");
    char *out1;
    do {
        //printf("\nout:%s\n",out_file);
        out1 = out;
        mkdir(out_file,S_IRWXU|S_IRWXG|S_IROTH);
        out = strtok(NULL,"/");
        sprintf(out_file+strlen(out_file),"/%s",out1);
    } while(out);
    //printf("\n1out:%s\n",out_file);
    FILE *output = fopen(out_file,"w");
    //printf("f:%s",out_file);
    if(!output)
        printf("error");
    else
        fprintf(output,"%s",content);
    fclose(output);
}

int main(int argc, char *argv[])
{
    query = argv[2];
    port = argv[6];
    host = argv[4];
    int p = atoi(argv[6]);
    const char root[100];
    strcpy(root,argv[2]);

    //create socket
    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        printf("Fail to create a socket.");
    }
    //struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;

    //localhost test
    info.sin_addr.s_addr = inet_addr(host);
    info.sin_port = htons(p);

    int err = connect(clientfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1) {
        printf("Connection error\n");
    }



    //send and receive
    sendnrecv(argv[2]);/*
    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        printf("Fail to create a socket.");
    }
    err = connect(clientfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1) {
        printf("Connection error\n");
    }
    sendnrecv(argv[2]);*/
    process(argv[2]);
    /*
        if((strcmp(type,"directory")==0) & content != NULL) {
            char *doc[20];
            doc[i] = strtok(content," ");
            while(doc[i] != NULL) {
                printf("\n%s ",doc[i]);
                i++;
                doc[i] = strtok(NULL," ");
            }
        } else {

        }*/



    close(clientfd);
    return 0;
}