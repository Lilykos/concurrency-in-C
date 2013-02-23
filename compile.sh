#!/bin/bash
#/* Processes with semaphores in C */
#/* Bash script for execution */

gcc -o main main.c
echo
gcc -o insert_process insert_process.c
echo
gcc -o delete_process delete_process.c
echo
gcc -o search_process search_process.c
