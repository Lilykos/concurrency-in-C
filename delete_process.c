/* Delete Process */

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

int semTake0(int );
int semGive0(int );
int semTake1(int );
int semGive1(int );


int main(int argc, char *argv[]) {
    int steps, shmid, sems;
    int i, flag, count2;
    float sleep_time;
    shMem* shm;
    long curtime;
    curtime = time(NULL);
    srand((unsigned int) curtime);
    
    steps = atoi(argv[1]);
    shmid = atoi(argv[2]);
    
    if((shm = (shMem *) shmat(shmid, NULL, 0)) == (shMem *) -1) {
        printf("Shmat error!\n");
        exit(1);
    }
    sems = shm->sems;
    
    
    for(i = 0; i < steps; i++) {
        flag = 0;									/* Flag for existent item in array */
        if(strcmp((shm->pinakas[0]), "") == 0) {
            flag = 1;
        }
        /* printf("delete %d waiting\n",getpid()); */
        if(semTake0(sems) == EXIT_FAILURE) { 
            printf("ERROR: semTake failed\n"); 
            return EXIT_FAILURE; 
        }
        if(semTake1(sems) == EXIT_FAILURE) { 
            printf("ERROR: semTake failed\n");
            return EXIT_FAILURE; 
        }
        
        
        /***************/
        /* Entering... */
        /***************/
        
        printf("++++++++++ Delete %d entered ++++++++++\n",getpid());
        count2 = shm->count;
        if((shm->count) > 0) {
            printf("Delete result: the %dth cell containing '%s'was deleted!\n", shm->count, shm->pinakas[count2-1]);
            strcpy((shm->pinakas[count2-1]), "");
            shm->count --;
        }
        else if((shm->count) == 0 && flag == 0) {
            printf("Delete result: the %dth cell containing '%s' was deleted!\n", shm->count, shm->pinakas[0]);
            strcpy((shm->pinakas[0]), "");
            flag = 1;
        }
        else if((shm->count) == 0 && flag == 1) {
            printf("Delete result: cannot delete, the array is empty!!!\n");
        }
        
        
        /***************/
        /* Leaving... */
        /***************/
        
        printf("++++++++++ Delete %d prepare to leave ++++++++++\n",getpid());
        if(semGive1(sems) == EXIT_FAILURE){ 
            printf("ERROR: semTake failed\n");  
            return EXIT_FAILURE;
        }
        if(semGive0(sems) == EXIT_FAILURE){ 
            printf("ERROR: semTake failed\n"); 
            return EXIT_FAILURE; 
        }
        sleep_time = (float)(rand()%150)/100;
        printf("++ Delete %d left and sleeping for %f secs ++\n",getpid(), sleep_time);
        sleep(sleep_time);
    }
    
    return 0;
}


int semTake0(int semid){ 					/* down the semaphore for delete and insert-delete */
    struct sembuf sops; 
    sops.sem_num = 0; 
    sops.sem_op = -1; /* semaphore operation */
    sops.sem_flg = SEM_UNDO; 
    if(semop(semid, &sops, 1) == -1) { 
        printf("semop error down: %s\n", strerror(errno));
        return EXIT_FAILURE; 
    } 
    return EXIT_SUCCESS; 
} 

int semGive0(int semid) { 					/* up the semaphore for delete and insert-delete */
    struct sembuf sops; 
    sops.sem_num = 0; 
    sops.sem_op = 1;
    sops.sem_flg = SEM_UNDO; 
    if(semop(semid, &sops, 1) == -1) { 
        printf("semop error down: %s\n", strerror(errno));
        return EXIT_FAILURE; 
    } 
    return EXIT_SUCCESS; 
}

int semTake1(int semid) { 					/* down the semaphore for search-delete */
    struct sembuf sops; 
    sops.sem_num = 1; 
    sops.sem_op = -1;
    sops.sem_flg = SEM_UNDO; 
    if(semop(semid, &sops, 1) == -1) { 
        printf("semop error down: %s\n", strerror(errno));
        return EXIT_FAILURE; 
    } 
    return EXIT_SUCCESS; 
} 

int semGive1(int semid) { 					/*up the semaphore for search-delete */
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
