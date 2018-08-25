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
#define MaxRoll 100

typedef void (*sighandler_t)(int);
int *ptr;
int shmid;

//Ctrl+C HANDLER
void disp_rem_kill(int signum){
	//DETACHING THE SHARED MEMORY!!
	shmdt((void *)ptr);  
	//KILLING THE PROCESS
    kill(0, SIGKILL);   
}


int main(int argc,char const *argv[]){
	int s;
	int rollno;
	//PROCESS ID
	int pid = getpid();

	if(argc!=2){
		printf("Usage:student rollno\n");
		return 0;
	}
	// GETTING ROLL NO FROM TERMINAL
	rollno  =atoi(argv[1]);
	if(rollno>MaxRoll||rollno<1){
		printf("Invalid Roll No.\n");
		return 0;
	}
	key_t mykey;
	// key_t ftok(const char *pathname, int proj_id);
	mykey  =ftok("/home/shubho/Documents/Operating System/Assign 1 feb/teacher.c",1);
	// int shmget(key_t key, size_t size, int shmflg);
	shmid = shmget(mykey,(2*sizeof(int)),IPC_CREAT|0777);
	// ATTACHING SHARED MEMORY TO STUDENT PROCESS.
	ptr = (int *)shmat(shmid,NULL,0);

	sighandler_t shandler;
	sighandler_t signal(int signum, sighandler_t handler);
    shandler =  signal(SIGINT, disp_rem_kill);
    //CHECKING FOR POINTER IS -1.
	while(*ptr != -1);
	//PASSING THE ROLLNO TO THE SHARED MEMORY
	*ptr = rollno;
	//PASSING THE PROCESSID TO THE SHARED MEMORY
	*(ptr+1) = pid;
	//DETACHING SHARED MEMORY FROM STUDENT PROCESS
	shmdt((void *)ptr);

	return 0;
}