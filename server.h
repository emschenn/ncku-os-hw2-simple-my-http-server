#ifndef SERVER_H
#define SERVER_H

typedef struct Queue {
    int capacity;
    int size;
    int front;
    int back;
    char** data;
} Queue;


void* newThreadWork(char msg);

void createQueue(int maxElements);
void dequeue();
char* pop();
void enqueue(char* msg);

void listFile(char* root);
char* getType(char *queryType);
void fileContent(char *file);
void respond(char *msg);


#endif
