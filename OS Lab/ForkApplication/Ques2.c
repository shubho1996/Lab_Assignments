#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]){
	char str[100] = "";
	int s;			//status
	pid_t p = 0;		// Temporary Process ID
	for (int i = 1; i <=argc-1; ++i){
		p = fork();	//Child Process Creation
		if(p==0){

			/*Child Process*/
			printf("Child Process No:%d.\n",i);
			strcat(str,argv[i]);	//Creating a string for exectuable
			char *myargv[5] = {str,NULL};
			//int execve(const char *filename, char *const argv[],char *const envp[]);
			//Error Handling of execve().
			if(execve(myargv[0],myargv,NULL)==-1){
				perror("execve failed :");
				exit(1);
			}
			else{
				//execve() executes the program pointed to by filename.
				execve(myargv[0],myargv,NULL);
			}

		}
		else{
			//Used to wait for state changes in a child of the calling process.
			wait(&s);

			if( WIFEXITED(s)){
				/* Normal Execution*/
				printf("Child executed Normally.\n");
				printf("The exit status of the child = %d.\n",WEXITSTATUS(s));
			}
			else{	/* Abnormal Execution*/
				printf("Child process executed Abnormally.\n");
			
			 	if(WIFSIGNALED(s)){
					printf("The child process was terminated by a signal.\n");
					printf("The number of the signal that caused the child process to terminate = %d\n", WTERMSIG(s));

					if(WCOREDUMP(s)){
					printf("The child produced  a  core  dump.\n");
					}
				}
			}
		}
	}
	return 0;
}
