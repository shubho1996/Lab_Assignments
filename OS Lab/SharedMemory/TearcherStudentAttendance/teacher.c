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
int arr[MaxRoll+1][2];
int *ptr;
int shmid;

//Ctrl+C HANDLER
void disp_rem_kill(int signum){
	//PRINTING ATTENDANCE REGISTAR
	printf("******Attendence Register[ProcessID=>RollNo]*********\n");
	for(int i=1;i<=MaxRoll;i++){
		if(arr[i][0]==1){
			printf("%d=>%d ",arr[i][1],i);
		}
	}
	printf("\n");
	shmdt((void *)ptr);     
	shmctl(shmid, IPC_RMID, NULL);
    kill(0, SIGKILL);   
}

int main(int argc,char const *argv[]){
	//DEFAULT VALUE IN REGISTAR
	for(int i=1;i<=MaxRoll;i++){
		arr[i][0]=-1;
	}

	key_t mykey;
	// key_t ftok(const char *pathname, int proj_id);
	mykey  =ftok("/home/shubho/Documents/Operating System/Assign 1 feb/teacher.c",1);
	// int shmget(key_t key, size_t size, int shmflg);
	shmid = shmget(mykey,(2*sizeof(int)),IPC_CREAT|0777);
	// ATTACHING SHARED MEMORY TO TEACHER PROCESS.
	ptr  = (int *)shmat(shmid,NULL,0);

	sighandler_t shandler;
	sighandler_t signal(int signum, sighandler_t handler);
    shandler =  signal(SIGINT, disp_rem_kill);

	*ptr = -1;
	int rollno;

	while(1){
		//CHECKING FOR ROLL NO.
		while(*ptr == -1);
		rollno = *ptr;
		//IF STUDENT ALREADY ATTENDED THE CLASS.
		if(arr[rollno][0]==1){
			printf("Already Attended.\n");
			*ptr = -1;
		}
		else{
			//ATTENDENCE IS MARKED IN REGISTAR.
			arr[rollno][0] = 1;
			arr[rollno][1] = *(ptr+1);
			//SHARED MEMORY INITIALIZED TO -1.
			*ptr = -1;
			printf("Attendence:%d\n", rollno);
			printf("Process ID:%d\n",arr[rollno][1]);
			//TEACHER CAN EITHER CONTINUE TO TAKE THE ATTENDANCE
			//PRINT THE ATTENDANCE REGISTAR
			//STOP TAKING ATTENDANCE
			printf("Press ENTER to continue,p to print the Register or q to quit:\n");
			char c=getchar();
			if(c == 'p'){
				//PRINTING ATTENDANCE REGISTAR
				printf("******Attendence Register[ProcessID=>RollNo]*********\n");
				for(int i=1;i<=MaxRoll;i++){
					if(arr[i][0]==1){
						printf("%d=>%d ",arr[i][1],i);
					}
				}
				printf("\n");
			}
			else if(c =='q')
				break;
			else
				continue;
		}
		
	}
	//PRINTING ATTENDANCE REGISTAR
	printf("******Attendence Register[ProcessID=>RollNo]*********\n");
	for(int i=1;i<=MaxRoll;i++){
		if(arr[i][0]==1){
			printf("%d=>%d ",arr[i][1],i);
		}
	}
	printf("\n");
	//DETACHING SHARED MEMORY FROM TEACHER PROCESS
	shmdt((void *)ptr);
	//DESTROYING SHARED MEMORY
	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}