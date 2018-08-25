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
#include "stackheader.h"

int main(int argc,char const *argv[]){
	int status,stackid,new;
	double  *ptr = (double *)malloc(sizeof(double));
	key_t key = ftok("/home/shubho/Documents/Networks/Assign2/Server2.c",1);
	status = shstackget(key,sizeof(double),5);
	if(status==-1)
		printf("shstackget:FAILED\n");
	else 
		stackid = status;

	*ptr = 50.85934;
	status = shstackpush(stackid,ptr);
	if(status!=0)
		printf("shstackpush:FAILED\n");
	else
		printf("Pushed Into Stack.\n");

	*ptr = 12.0923;
	status = shstackpush(stackid,ptr);
	if(status!=0)
		printf("shstackpush:FAILED\n");
	else
		printf("Pushed Into Stack.\n");

	*ptr = 0;
	status = shstackpop(stackid,ptr);
	if(status!=0)
		printf("shstackpop:FAILED\n");
	else
		printf("Poped element = %lf\n",*ptr);


	status = shstackrm(stackid,SHSTACK_CMPL);
	if(status==-1)
		printf("shstackrm:FAILED\n");
	else
		printf("Stack Deleted Successfully.\n");

	// status = shstackrm(stackid,SHSTACK_DETACH);
	// if(status==-1)
	// 	printf("shstackrm:FAILED\n");
	// else
	// 	printf("Stack Detach Successfully.\n");

	return 0;
}