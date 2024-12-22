#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdint.h>

typedef struct {
    sem_t runway;
    sem_t tower;
    sem_t queueLock;
    int queueArr[10];
} Airport;

typedef struct {
    Airport* airport;
    int planeID;
}planeInfo;

Airport* init_Airport() {
    Airport* airport = malloc(sizeof(Airport));
    sem_init(&airport->runway, 0, 3); // 3 runways available
    sem_init(&airport->tower, 0, 1);  // Single tower
    sem_init(&airport->queueLock, 0, 1);  // Protect the queue
    for (int i = 0; i < 10; i++) {
        airport->queueArr[i] = 0;
    }
    return airport;
}

planeInfo* init_planeInfo(Airport* airport, int ID){
    planeInfo* info = malloc(sizeof(planeInfo));
    info->airport = airport;
    info->planeID = ID;
}

void takeOff(Airport* airport, int planeID) {
    int sval;
    sem_getvalue(&airport->tower, &sval);
    
    //printf("take off time? plane %i, current sem is %i, current airport pointer %lld \n", planeID, sval, (long long int)airport);
    sem_wait(&airport->tower);
    //printf("tower locked plane# %i \n", planeID);
    sem_wait(&airport->runway);
    printf("Plane %d is taking off\n", planeID);
    sleep(1);
    sem_post(&airport->runway);
    sem_post(&airport->tower);
    //printf("Plane %d has taken off and released resources\n", planeID);
}

void enterQueue(Airport* airport, int planeID) {
    sem_wait(&airport->queueLock); // Protect queue modification
    for (int i = 0; i < 10; i++) {
        if (airport->queueArr[i] == 0) {
            airport->queueArr[i] = planeID;
            printf("Plane %d entered the queue at position %d\n", planeID, i + 1);
            sleep(1);
            break;
        }
    }
    sem_post(&airport->queueLock);
}

void* plane(planeInfo* info) {
    int planeID = info->planeID;
    Airport* airport = info->airport;

    sleep(rand() % 3 + 1); // Simulate delay
    enterQueue(airport, planeID);
    sleep(rand() %3 +1);
    while (1) {
        sem_wait(&airport->queueLock);
        //printf("queuelock aquiured fopr plane %i", planeID);
        if (airport->queueArr[0] == planeID) {
            airport->queueArr[0] = 0; // Remove from queue
            for (int i = 0; i < 9; i++) { // Shift queue forward
                airport->queueArr[i] = airport->queueArr[i + 1];
                airport->queueArr[i + 1] = 0;
            }
            sem_post(&airport->queueLock);
            break;
        }
        sem_post(&airport->queueLock);
    }
    
    takeOff(airport, planeID);
    return NULL;
}

void* queueHandler(void* args) {
    Airport* airport = (Airport*)args;
    while (1) {
        sem_wait(&airport->queueLock);
        for (int i = 0; i < 9; i++) {
            if (airport->queueArr[i] == 0) {
                airport->queueArr[i] = airport->queueArr[i + 1];
                airport->queueArr[i + 1] = 0;
            }
        }
        sem_post(&airport->queueLock);
        sleep(1); // Prevent busy-looping
        for(int j = 0; j < 5; j++){
            printf("Queue position %i is plane %i \n", j + 1, airport->queueArr[j]);
            //printf("the curernt queue is plane %i, plane %i, plane %i, plane %i, plane %i \n", airport->queueArr[0],airport->queueArr[1],airport->queueArr[2],airport->queueArr[3],airport->queueArr[4]);
        }
        //printf("\n");
    }
    return NULL;
}

int main() {
    printf("Main start\n");
    srand(time(NULL)); // Seed for random delays
    Airport* airport = init_Airport();

    pthread_t queueThread;
    pthread_create(&queueThread, NULL, queueHandler, airport);

    int sval;
    sem_getvalue(&airport->tower, &sval);
    //printf("cur sem vlaue %i \n", sval);

    pthread_t planeThreads[5];
    int args[6]; // Plane IDs
    for (int i = 0; i < 5; i++) {
        //long long int* threadArgs = malloc(2 * sizeof(long long int)); // Plane ID + Airport Pointer
        //threadArgs[0] = i + 1;
        //Airport* airport = (Airport*)(((int*)args) + 1);
        //printf("this is the airport being send to thread %i, address %lld \n", i, (long long int)airport);
        //*(Airport**)(threadArgs + 1) = airport;
        planeInfo* info = init_planeInfo(airport, (i+1));
        pthread_create(&planeThreads[i], NULL, (void*)plane, info);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(planeThreads[i], NULL);
    }

    pthread_cancel(queueThread);
    pthread_join(queueThread, NULL);

    free(airport);
    return 0;
}
