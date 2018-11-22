#ifndef CLIENT_H
#define CLIENT_H



void createFolder(char* root);
void createFile(char* root,char* content);
void sendnrecv();
void process(char* root);
void* threadWork(char* newQuery);

#endif
