#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <pthread.h>

char response[5000];
char *query;
char *port;
char *host;

void* sendnrecv(clientfd)
{
    printf("\nhello\n");
    memset(response,0,sizeof(response));
    char request[100];
    sprintf(request,"GET %s HTTP/1.x\r\nHOST: %s:%s\r\n\r\n",query,host,port);
    printf("repuest:\n%s",request);
    send(clientfd,request,strlen(request),0);//send(clientfd,request,strlen(request),0);
    printf("test");
    recv(clientfd,response,sizeof(response),0);
    printf("%s",response);
}

void thread(char *msg)
{

}

int main(int argc, char *argv[])
{
    query = argv[2];
    port = argv[6];
    host = argv[4];
    int clientfd = 0;
    int p = atoi(argv[6]);
    const char root[50];
    strcpy(root,argv[2]);

    //create socket
    clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1) {
        printf("Fail to create a socket.");
    }
    struct sockaddr_in info;
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
    sendnrecv(clientfd);

    char response2[0xfff];
    strcpy(response2,response);
    int i=0;
    char *type = strtok(response,"\n");
    type = strtok(NULL," ");
    type = strtok(NULL,"\r");
    char *content = strtok(NULL,"\n");
    content = strtok(NULL,"\0");

    //create output
    if(type != NULL) {
        char out_file[70] = {};
        mkdir("output",S_IRWXU|S_IRWXG|S_IROTH);
        if(strcmp(type,"directory")==0) {
            char *out = strtok(root,"/");  //path: output/../..
            sprintf(out_file,"output/%s",out);
            while(out) {
                printf("\nout:%s\n",out_file);
                mkdir(out_file,S_IRWXU|S_IRWXG|S_IROTH);
                out = strtok(NULL,"/");
                sprintf(out_file+strlen(out_file),"/%s",out);
            }
        } else {
            char *out = strtok(root,"/");  //path: output/../..
            strcat(out_file,"output");
            char *out1;
            do {
                printf("\nout:%s\n",out_file);
                out1 = out;
                mkdir(out_file,S_IRWXU|S_IRWXG|S_IROTH);
                out = strtok(NULL,"/");
                //out1 = strtok(NULL,"/");
                sprintf(out_file+strlen(out_file),"/%s",out1);
            } while(out);
            //strcat(out_file,out1);
            printf("\n1out:%s\n",out_file);
            FILE *output = fopen(out_file,"w");
            //printf("f:%s",out_file);
            if(!output)
                printf("error");
            else
                fprintf(output,"%s",content);
            fclose(output);
        }
    }

    if((strcmp(type,"directory")==0) & content != NULL) {
        char *doc[20];
        doc[i] = strtok(content," ");
        while(doc[i] != NULL) {
            printf("\n%s ",doc[i]);
            i++;
            doc[i] = strtok(NULL," ");
        }
    } else {

    }



    close(clientfd);
    return 0;
}