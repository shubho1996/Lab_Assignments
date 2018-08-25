#ifndef STACKHEADER_H
#define STACKHEADER_H

#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <stdbool.h>
#define Max_Stack 10         		/*Maximum No. of Stack.*/
#define SHSTACK_RMID 11011			/*Delete the associated stack.*/
#define SHSTACK_DETACH 00100		/*Detach the associated stack.*/
#define SHSTACK_CMPL 11111			/*Completely remove everything*/
#define P(s) semop(s, &Pop, 1);
#define V(s) semop(s, &Vop, 1);

struct sembuf Pop;
struct sembuf Vop;

struct stackdesc *ptr;
int stkdescshmid,semid,stackid,universe;

struct stackdesc
{
	key_t stackKey;
	int shmid;
	size_t data_size;
	int stack_size;
	void *stack_ptr;
	void *stack_top;
	bool free;
	int num;
};

union semun {
    int val;    			 /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
} setvalArg;

void initialize();
void semopset(int);
void structcreation();
void* stackcreate(int,key_t,size_t,int);
int shstackget(key_t,size_t,int);
int shstackpush(int,void *);
int shstackpop(int,void *);
int shstackrm(int,int);

#endif
