#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]){
	char str[100] = ""; 
	int s;
	pid_t p = 0;
	for (int i = 1; i <=argc-1; ++i){
		p = fork();	//Child Process Creation
		if(p==0){

			/*Child Process*/
			strcat(str,argv[i]);		//Creating a string for exectuable
			char *myargv[5] = {str,NULL};

			//int execve(const char *filename, char *const argv[],char *const envp[]);
			//Error Handling of execve().
			if(execve(myargv[0],myargv,NULL)==-1){
				perror("execve failed");
				exit(1);
			}
			else{
				//execve() executes the program pointed to by filename.
				execve(myargv[0],myargv,NULL);
			}

		}

		//Used to wait for state changes in a child of the calling process.
		wait(&s);
	}

	return 0;
}
