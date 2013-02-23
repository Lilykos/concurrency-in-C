/* Processes with semaphores in C */
/* Main file */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include "dhlwseis.h"

#define PERUS 0666
#define semkey (key_t) 7000
#define shmkey (key_t) 8000

int main(int argc, char *argv[]) {
    int status, lines, shmid, pid;
    int i, j;
    int i_num, s_num, d_num;
    char shmstr[32];								/* string containing the semaphores location */
    char *s; 										/* allocation of space for fgets() */
    char* lines_str = malloc(64 * sizeof(char)); 	/* for the exec functions, we give lines as arguments */
    union semun arg;
    
    shMem* shm;
    FILE *f;
    
    if (argc != 5) {
        printf("Wrong number of arguments!");
        exit(1);
    }
    i_num=atoi(argv[2]);
    s_num=atoi(argv[3]);
    d_num=atoi(argv[4]);
    
    s = malloc(256 * sizeof(char));
    lines = 0;	
    if((f = fopen("text_file.txt","r")) == NULL) {
        printf("Cannot open the file!\n");
        exit(1);
    }
    
    while ((fgets(s,4098, f)) != NULL) {
        lines ++;
    }
    sprintf(lines_str, "%d", lines);
    
    
    if ((shmid = shmget(shmkey, (sizeof(shMem)), PERUS|IPC_CREAT)) == -1) {	/* shared memory */
        perror("Shmget error\n");
        exit(1);
    }
    if((shm = (shMem *) shmat(shmid,0,0)) == (shMem *) -1) {
        perror("Shmat error\n");
        exit(1);
    }
    
    
    if((shm->sems = semget((key_t) semkey, 3, PERUS|IPC_CREAT)) == -1) {	/* semaphores */
        perror("Semget error\n");
        exit(1);
    }  
    arg.val=1; /* arxikopoihsh semaphores */
    if((semctl(shm->sems, 0, SETVAL, arg)) == -1) {
        perror("semctl error");
        exit(1);
    }
    arg.val = 1;
    if((semctl(shm->sems, 1, SETVAL, arg)) == -1) {
        perror("semclt error");
        exit(1);
    }
    arg.val = 1;
    if((semctl(shm->sems, 2, SETVAL, arg)) == -1) {
        perror("semctl error");
        exit(1);
    }
    
    
    shm->condvar = 1;				/* condition for Search function */
    shm->count = 0;					/* counter for insert/delete for array check */
    sprintf(shmstr, "%d", shmid);	/* sems to string for exec function */
    
    for(i = 0; i < megethos; i++) {	/* initialize the array with "" (blank) */
        strcpy(shm->pinakas[i], "");
    }
    
    /*************************/
    /* Create the processes! */
    /*************************/
    
    for(i = 0; i < i_num; i++) { 	/* Insert Process */
        pid = fork();
        if(pid == 0) {
            execlp("./insert_process", "./insert_process", argv[1], shmstr, lines_str, NULL);
            printf("Error exec-insert: %s!\n", strerror(errno));
            exit(1);
        }
        else if(pid == -1) {
            printf("Fork error!\n");
            exit(1);
        }
    }
    
    for(i = 0; i < s_num; i++) { 	/* Search Process */
        pid = fork();
        if(pid == 0) {
            execlp("./search_process", "./search_process", argv[1], shmstr, NULL);
            printf("Error exec-search: %s!\n", strerror(errno));
            exit(1);
        }
        else if(pid==-1) {
            printf("Fork error!\n");
            exit(1);
        }
    }
    
    for(i = 0; i < d_num; i++) {	/* Delete Process */
        pid = fork();
        if(pid == 0) {
            execlp("./delete_process", "./delete_process", argv[1], shmstr, NULL);
            printf("Error exec-delete: %s!\n", strerror(errno));
            exit(1);
        }
        else if(pid==-1) {
            printf("!Fork error!\n");
            exit(1);
        }
    }
    
    
    /* Wait until every child is finished. */
    
    for(j = 0; j < i_num + d_num + s_num; j++) { 
        pid = wait(&status);
        printf("Child with pid %d exited sucessfully!\n", pid);
    }
    
    
    /* deallocate shared memory and semaphores */
    
    fclose(f);  
    if((semctl(shm->sems, 0, IPC_RMID) )== -1){
        printf("Semctl error!\n");
        exit(1);
    }
    if((shmctl(shmid, IPC_RMID, 0)) == -1){
        printf("Shmctl error\n");
        exit(1);
    }
    
    return 0;
}
