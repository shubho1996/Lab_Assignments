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

typedef void (*sighandler_t)(int);

int a[100][100],m,n,max,min,d;
sighandler_t shandler;
int shmid;

void sighandler(int signum){
	//REMOVING THE SHARED MEMORY!!
	shmctl(shmid, IPC_RMID, NULL);
	//Now killing the process
    kill(0, SIGKILL);   
}

int avgmat(int p, int q){
	int res;
	//HANDLING SINGLE ROW AND SINGLE COLUMN
	//SINGLE ROW
	if(m==1){
		//SINGLE CLOUMN
		if(n==1)
			return a[p][q];
		//TWO COLUMNS
		else if(n==2){
			if(q==0)
				return (a[p][q]+a[p][q+1])/2;

			else
				return (a[p][q]+a[p][q-1])/2;
		}
		//MULTIPLE COLUMNS
		else{
			if(q==0)
				return ((a[p][q]+a[p][q+1])/2);

			else if(q==(n-1))
				return ((a[p][q]+a[p][q-1])/2);

			else 
				return ((a[p][q]+a[p][q-1]+a[p][q+1])/3);

		}
	}
	//SINGLE COLUMN AND MULTIPLE ROW
	else if(n==1){
		//TWO ROWS
		if(m==2){
			if(p==0)
				return (a[p][q]+a[p+1][q])/2;

			else
				return (a[p][q]+a[p-1][q])/2;
		}
		//MULTIPLE ROWS
		else{
			if(p==0)
				return ((a[p][q]+a[p+1][q])/2);
	
			else if(p==(n-1))
				return ((a[p][q]+a[p-1][q])/2);
	
			else
				return ((a[p][q]+a[p-1][q]+a[p+1][q])/3);
		}
	}


	//HANDLING MULTIPLE ROW AND MULTIPLE COLUMN
	
	//FIRST ROW
	else if(p==0){
		//FIRST COLUMN IN FIRST ROW
		if(q==0)
			res = (a[p][q]+a[p][q+1]+a[p+1][q]+a[p+1][q+1])/4;
		//LAST CLOUMN IN FIRST ROW
		else if(q==(n-1))
			res = (a[p][q]+a[p][q-1]+a[p+1][q]+a[p+1][q-1])/4;
		// REMAINING ELEMENTS IN FIRST ROW
		else
			res = (a[p][q-1]+a[p][q]+a[p][q+1]+a[p+1][q-1]+a[p+1][q]+a[p+1][q+1])/6;
		
	}
	//LAST ROW
	else if(p==(m-1)){
		//FIRST COLUMN IN LAST ROW
		if(q==0)
			res = (a[p][q]+a[p][q+1]+a[p-1][q]+a[p-1][q+1])/4;
		//LAST CLOUMN IN LAST ROW
		else if(q==(n-1))
			res = (a[p][q]+a[p][q-1]+a[p-1][q]+a[p-1][q-1])/4;
		// REMAINING ELEMENTS IN LAST ROW
		else
			res = (a[p][q-1]+a[p][q]+a[p][q+1]+a[p-1][q-1]+a[p-1][q]+a[p-1][q+1])/6;
		
	}
	// FIRST COLUMN REMAINING ELEMENTS
	else if(q==0)
		res = (a[p][q]+a[p+1][q]+a[p-1][q]+a[p][q+1]+a[p+1][q+1]+a[p-1][q+1])/6;
	
	//LAST COLUMN REMAINING ELEMENTS
	else if(q==(n-1))
		res = (a[p][q]+a[p+1][q]+a[p-1][q]+a[p][q-1]+a[p+1][q-1]+a[p-1][q-1])/6;
	
	//REMAING ELEMENTS
	else
		res = (a[p][q]+a[p][q-1]+a[p][q+1]+a[p-1][q]+a[p-1][q-1]+a[p-1][q+1]+a[p+1][q]+a[p+1][q-1]+a[p+1][q+1])/9;
				

	return res;
	
}

int main(int argc,char const *argv[]){
	int s;
	int *ptr;
	//sighandler_t signal(int signum, sighandler_t handler);
	shandler =  signal(SIGINT, sighandler);

	//INPUT ACCCEPTING
	printf("Enter the dimension of \"a\" matrix[ROWS][COLUMNS]:");
	scanf("%d %d",&m,&n);

	printf("Enter the elements of matrix.\n");
	for(int i=0;i<m;i++){
		for (int j=0;j<n ; ++j)	
			scanf("%d",&a[i][j]);	
	}
	//PRINITNG INPUT MATRIX
	printf("Input Matrix.\n");
	for(int i=0;i<m;i++){
		for(int j=0;j<n;j++){
			printf(" %d ",a[i][j]);
		}
		printf("\n");
	}

	//SHARED MEMORY ALLOCATION
	key_t key ;
	size_t size = sizeof(sizeof(int)*m*n);
	
	//int shmget(key_t key, size_t size, int shmflg);

	shmid =  shmget(IPC_PRIVATE,size,IPC_CREAT|0777); 
	// FORKING CHILD PROCESS
	int p;
	for (int i = 0; i < m; ++i){
		for (int j = 0; j < n; ++j){
			p = fork();
			if(p==0){
				// Child Process
				int c=0;
				//ATTACHING SHARED MENORY IN CHILD PROCESS
				ptr = (int *)shmat(shmid,NULL,0);
				//CALL TO FUNCTION AVGRAGE OF MATRIX AT i,j.
				c = avgmat(i,j);
				// STORING THE VALUE TO SHARED VARIABLE.
				int *loc = ptr+(i*n+j);
				*loc  = c; 
				//DETACHING SHARED MEMORY FROM CHILD PROCESS
				shmdt((void *)ptr);
				exit(0);
			}
			else
				//PARENT WAITING FOR CHILD TO EXIT
				wait(&s);
		}
	}	
	
	
	if(p!=0){

		//ATTACHING SHARED MEMORY IN PARENT PROCESS
		ptr = shmat(shmid,NULL,0);
		printf("Resulting Matrix.\n");
		for(int i=0;i<m;i++){
			for(int j=0;j<n;j++){
				int *loc = ptr + (i*n+j);
				printf(" %d ",*loc);
			}
			printf("\n");
		}
		//DETACHING SHARED MEMORY FROM PARENT PROCESS
		shmdt((void *)ptr);
		//DEALLOCATION OF SHARED MEMORY
		shmctl(shmid, IPC_RMID, NULL);

	}
	return 0;
}
