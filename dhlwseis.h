#ifndef _dhlwseis_H
#define _dhlwseis_H

#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#define megethos 200

union semun {
   int val;
   struct semid_ds *buf;
   unsigned short int *array;
};

typedef struct {
    char pinakas[megethos][150];
    int count;
    int sems;
    int condvar;	
}shMem;

#endif
