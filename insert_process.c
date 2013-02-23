/* Insert Process */

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

int semTake0(int );
int semGive0(int );


int main(int argc, char *argv[]) {
    int steps, shmid, sems;
    int i, j, k, count2, lines;
    char stoixeio[150];				/* the line will be stored here */
    float sleep_time;
    shMem* shm;
    FILE *f;
    long curtime;
    curtime = time(NULL);
    srand((unsigned int) curtime);
    
    steps = atoi(argv[1]);
    shmid = atoi(argv[2]);
    lines = atoi(argv[3]);
    
    if((shm = (shMem *) shmat(shmid, NULL, 0)) == (shMem *) -1) {
        printf("Shmat error!\n");
        exit(1);
    }
    sems = shm->sems;
    
    if((f = fopen("text_file.txt","r")) == NULL) {
        printf("Cannot open the file!\n");
        exit(1);
    }
    
    for(i = 0; i < steps; i++) {
        /* printf("Insert %d waiting\n",getpid()); */
        if(semTake0(sems) == EXIT_FAILURE) { 
            printf("ERROR: semTake failed\n");
            return EXIT_FAILURE; 
        }
        
        
        /***************/
        /* Entering... */
        /***************/
        
        printf("////////// Insert %d entered //////////\n",getpid());
        j = rand()%lines + 1;			/* random number to find the line number that will be inserted */
        for(k = 0; k < j; k++) {
            fgets(stoixeio, 4096, f);
        }
        
        count2 = shm->count;
        if((shm->count) <= (megethos - 1)) {
            strcpy(shm->pinakas[count2], stoixeio);
            shm->count ++;
            printf("Insert result: the line %d fron the text was inserted in the array and the text inserted was --> %s", j, shm->pinakas[count2]);
        }
        else {
            printf("Insert result: cannot insert, the array is full!!!\n");
        }
        
        
        /***************/
        /* Leaving... */
        /***************/
        
        printf("////////// Insert %d prepare to leave //////////\n",getpid());
        if(semGive0(sems) == EXIT_FAILURE) { 
            printf("ERROR: semTake failed\n");
            return EXIT_FAILURE; 
        }
        sleep_time = (float)(rand()%150)/100;
        printf("// Insert %d left and sleeping for %f secs //\n",getpid(), sleep_time);
        sleep(sleep_time);
    }
    
    return 0;
}


int semTake0(int semid) { 					/* down o semaphore gia insert kai insert-delete */
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

int semGive0(int semid) { 					/* up o semaphore gia insert kai insert-delete */
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
