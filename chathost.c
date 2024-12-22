#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct{
    char* buffer[256]; // store the messages going bacck and forth
    int buffFull; //used to determine if there is currently a message to be read in the buffer
    int outGoing;// used to determine if the message in the buffer is coming or going from the client
    sem_t buffEdit; // prevent over writing in the buffer, read is safe tho
}client;

client* init_client(){
    client* client = malloc(sizeof(client));
    sem_init(&client->buffEdit, 0 ,1);
    client->buffFull = 0;
    client->buffer[256] = "hello"; 
    client->outGoing = 0;
}

void Client1(client* client){

    for(int i = 0; i < 10; i++){
        if(i == 0){
            sem_wait(&client->buffEdit);
            client->buffFull = 1;
            client->buffer[256] = "Client 1 Connected \0";
            client->outGoing = 1;
            sem_post(&client->buffEdit);
        }
        if((client->buffFull == 1) && (client->outGoing == 0)){
            printf("%s", client->buffer[256]);
            client->buffFull = 0;
        }
        sleep(1);
        if(i == 4){
            sem_wait(&client->buffEdit);
            client->buffFull = 1;
            client->buffer[256] = "hello all \0";
            client->outGoing = 1;
            sem_post(&client->buffEdit);
        }
         if(i == 9){
            sem_wait(&client->buffEdit);
            client->buffFull = 1;
            client->buffer[256] = "Client 1 Disconnected \0";
            client->outGoing = 1;
            sem_post(&client->buffEdit);
        }

    }
    return;

}

void Client2(client* client){

  for(int i = 0; i < 10; i++){
        if(i == 0){
            sem_wait(&client->buffEdit);
            client->buffFull = 1;
            client->buffer[256] = "Client 2 Connected \0";
            client->outGoing = 1;
            sem_post(&client->buffEdit);
        }
        if((client->buffFull == 1) && (client->outGoing == 0)){
            printf("%s", client->buffer[256]);
            client->buffFull = 0;
        }
        sleep(1);
        if(i == 4){
            sem_wait(&client->buffEdit);
            client->buffFull = 1;
            client->buffer[256] = "hello client 1 \0";
            client->outGoing = 1;
            sem_post(&client->buffEdit);
        }
        if(i == 9){
            sem_wait(&client->buffEdit);
            client->buffFull = 1;
            client->buffer[256] = "Client 1 Disconnected \0";
            client->outGoing = 1;
            sem_post(&client->buffEdit);
        }
    }
    return;
}

int main(){
    printf("hello from main");
    int sentBy = 0;
    char* transfer[256] = {0};
    transfer[256] = "\0";
    client* client1 = init_client();
    client* client2 = init_client();

    pthread_t clientThread1, clientThread2;
    client1->buffer[256] = "Connected to Server\0";
    client1->buffFull = 1;
    pthread_create(&clientThread1,NULL, (void*)Client1, client1);
    
    client2->buffer[256] = "Connected to Server\0";
    client2->buffFull = 1;
    pthread_create(&clientThread2,NULL, (void*)Client2, client2);

    while(1){
        if(client1->outGoing == 1){
            sem_wait(&client1->buffEdit);
            transfer[256] = client1->buffer[256];
            client1->outGoing = 0;
            client1->buffFull = 0;
            sem_post(&client1->buffEdit);
            //printf("Client 1: %s ", transfer[256]); dont print here send to client2

            sentBy = 1;
        }
        else if(client2->outGoing == 1){
            sem_wait(&client2->buffEdit);
            transfer[256] = client2->buffer[256];
            client2->outGoing = 0;
            client2->buffFull = 0;
            sem_post(&client2->buffEdit);
            //printf("Client 2: %s", transfer[256]);
            sentBy = 2;
        }
        if(sentBy == 1){
            sem_wait(&client2->buffEdit);
            client2->buffer[256] = transfer[256];
            client2->buffFull = 1;
            sem_post(&client2->buffEdit);
            sentBy = 0;
        }
        if(sentBy == 2){
            sem_wait(&client1->buffEdit);
            client1->buffer[256] = transfer[256];
            client1->buffFull = 1;
            sem_post(&client1->buffEdit);
            sentBy = 0;
        }
    }
    return 1;
}