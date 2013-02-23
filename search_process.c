/* Search Process */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include "dhlwseis.h"

#define megethos 200

int semTake1(int );
int semGive1(int );
int semTake2(int );
int semGive2(int );


int main(int argc, char *argv[]) {
    int steps, shmid, sems,
    int i, j;
    float sleep_time;
    shMem* shm;
    long curtime;
    curtime = time(NULL);
    srand((unsigned int) curtime);
    
    steps = atoi(argv[1]);
    shmid = atoi(argv[2]);
    
    if((shm = (shMem *) shmat(shmid, NULL, 0)) == (shMem *) -1) {
        printf("shmat error\n");
        exit(1);
    }
    sems = shm->sems;	

    for(i = 0; i < steps; i++) {
        /* printf("search %d waiting\n",getpid()); */
        if(semTake2(sems) == EXIT_FAILURE) { 
            printf("ERROR: semTake failed\n"); 
            return EXIT_FAILURE; 
        }
        if(shm->condvar == 1) {
            if(semTake1(sems) == EXIT_FAILURE) { 
                printf("ERROR: semTake failed\n");
                return EXIT_FAILURE; 
            }
            shm->condvar = 0;
        }
        if (semGive2(sems) == EXIT_FAILURE) { 
            printf("ERROR: semTake failed\n"); 
            return EXIT_FAILURE; 
        }
        
        
        /***************/
        /* Entering... */
        /***************/
        
        printf("---------- SEARCH %d ENTERED ----------\n",getpid());
        j = rand()%199;
        if(strcmp(shm->pinakas[j], "") == 0) {
            printf("Search result: the %dth cell of the array was found empty!\n",j);
        }
        else {
            printf("Search result: in the %dth cell of the array the string --> %s was found!\n", j, shm->pinakas[j]);
        }
        
        
        /***************/
        /* Leaving... */
        /***************/
        
        printf("---------- SEARCH %d LEFT ----------\n",getpid());
        if(semTake2(sems) == EXIT_FAILURE) { 
            printf("ERROR: semTake failed\n"); 
            return EXIT_FAILURE; 
        }
        if(shm->condvar == 0) {
            if(semGive1(sems) == EXIT_FAILURE) { 
                printf("ERROR: semTake failed\n"); 
                return EXIT_FAILURE;
            }
            shm->condvar = 1;
        }
        if(semGive2(sems) == EXIT_FAILURE) { 
            printf("ERROR: semTake failed\n");
            return EXIT_FAILURE; 
        }
        sleep_time = (float)(rand()%150)/100;
        printf("-- Search %d left and sleeping for %f secs --\n",getpid(), sleep_time);
        sleep(sleep_time);
    }
    
    return 0;
}

int semTake1(int semid) { 				/*down the semaphore for search-delete */
    struct sembuf sops; 
    sops.sem_num = 1; 
    sops.sem_op = -1; /* semaphore operation */
    sops.sem_flg = SEM_UNDO; 
    if(semop(semid, &sops, 1) == -1) { 
        printf("semop error down: %s\n", strerror(errno));
        return EXIT_FAILURE; 
    } 
    return EXIT_SUCCESS; 
} 

int semGive1(int semid) { 				/* up the semaphore for search-delete */
    struct sembuf sops; 
    sops.sem_num = 1;
    sops.sem_op = 1;
    sops.sem_flg = SEM_UNDO; 
    if(semop(semid, &sops, 1) == -1) { 
        printf("semop error down: %s\n", strerror(errno));
        return EXIT_FAILURE; 
    } 
    return EXIT_SUCCESS; 
}

int semTake2(int semid) { 				/* down the semaphore for search only */
    struct sembuf sops; 
    sops.sem_num = 2;
    sops.sem_op = -1;
    sops.sem_flg = SEM_UNDO; 
    if(semop(semid, &sops, 1) == -1) { 
        printf("semop error down: %s\n", strerror(errno));
        return EXIT_FAILURE; 
    } 
    return EXIT_SUCCESS; 
} 

int semGive2(int semid){ 				/* up the semaphore for search only */
    struct sembuf sops; 
    sops.sem_num = 2; 
    sops.sem_op = 1;
    sops.sem_flg = SEM_UNDO; 
    if(semop(semid, &sops, 1) == -1) { 
        printf("semop error down: %s\n", strerror(errno));
        return EXIT_FAILURE; 
    } 
    return EXIT_SUCCESS; 
}
