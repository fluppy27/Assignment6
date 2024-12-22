#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct{
    sem_t runway;
    sem_t tower;
} Airport;

Airport* init_Airport(){
    Airport* airport = malloc(sizeof(Airport));
    sem_init(&airport->runway,0,3);
    sem_init(&airport->tower,0,1);
    return airport;
}

void takeOff(Airport* airport, int id){
    sem_wait(&airport->runway);
    sem_wait(&airport->tower);
    printf("plane%i is taking off \n", id);
    sleep(1);
    sem_post(&airport->tower);
    sem_post(&airport->runway);
}

void plane1(Airport* airport){
    int id = 1;
    for(int i = 0; i < 5; i ++){
        sleep(2);
        printf("plane%i is requesting take off \n", id);
        takeOff(airport,1);
        sleep(3);
    }
}
void plane2(Airport* airport){
    int id = 2;
    for(int i = 0; i < 5; i ++){
        sleep(1);
        printf("plane%i is requesting take off\n", id);
        takeOff(airport,2);
        sleep(2);
    }
}
void plane3(Airport* airport){
    int id = 3;
    for(int i = 0; i < 5; i ++){
        sleep(3);
        printf("plane%i is requesting take off \n", id);
        takeOff(airport,3);
        sleep(4);
    }
}
void plane4(Airport* airport){
    int id = 4;
    for(int i = 0; i < 5; i ++){
        sleep(5);
        printf("plane%i is requesting take off \n", id);
        takeOff(airport,4);
        sleep(4);
    }
}
void plane5(Airport* airport){
    int id = 5;
    for(int i = 0; i < 5; i ++){
        sleep(2);
        printf("plane%i is requesting take off \n", id);
        takeOff(airport,5);
        sleep(3);
    }
}

int main(){
    Airport* airport = init_Airport();

    pthread_t thread1, thread2, thread3, thread4, thread5;

    pthread_create(&thread1, NULL,(void*)plane1,airport);
    pthread_create(&thread2, NULL,(void*)plane2,airport);
    pthread_create(&thread3, NULL,(void*)plane3,airport);
    pthread_create(&thread4, NULL,(void*)plane4,airport);
    pthread_create(&thread5, NULL,(void*)plane5,airport);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);
    pthread_join(thread5, NULL);

}