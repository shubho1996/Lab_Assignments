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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
//#include <readline/readline.h>
//#include <readline/history.h>
#define clear() printf("\033[H\033[J")
//Flag1----/&---1/&&---2/|---3(front)/||---4/;---5/>---6/<---7/|---8(back)/|command|---9/------------FLAG VALUES
//Flag2----Status.

// stdin ---- 0
// stdout --- 1
// stderr --- 2

int breakcommand(char *);
void decode(int ,int,int *,int *);
int runcommand(int,int,int *);
int fileexec(int , const char **);
int commandnum,pd1[2],pd2[2],pipecount=0;
char command[100][100];
char tempcommand[50][100];

int main(int argc,char const *argv[]){
	clear();
	int status; 
	int ret;
	if(argc>1)
		fileexec((argc-1),argv);
	while(1){
		wait(&status);
		int len;
		char input[100],*line;
		char cwd[1000];
		getcwd(cwd,sizeof(cwd));
		printf("\x1B[36m%s~>\x1B[0m",cwd);
		fgets(input,100,stdin);
		if(strcmp(input,"\n")==0)
			continue;
		if(strcmp(input,"exit\n")==0)
			exit(0);
		commandnum = breakcommand(input);
		int flag1=0,flag2=0;
		for(int i=0;i<commandnum;i++){
			memset(tempcommand,'\0',sizeof(char)*50*100);
			int num = 0;
			for(int j=0;;j++){
				if(strcmp(command[i],"&")==0||strcmp(command[i],"|")==0||strcmp(command[i],"&&")==0||strcmp(command[i],"||")==0||strcmp(command[i],";")==0||strcmp(command[i],">")==0||strcmp(command[i],"<")==0){
					if(strcmp(command[i],"&")==0)
						flag1 = 1;
					else if(strcmp(command[i],"|")==0){
						if(flag1==8)
							flag1 = 9;
						else 
							flag1 = 3;
						break;
					}
					else if(strcmp(command[i],">")==0)
						flag1 = 6;
					
					else if(strcmp(command[i],"<")==0)
						flag1 = 7;
					pipecount=0;
					break;
				}
				else{
					strcpy(tempcommand[j],command[i]);
					i++;
					num++;
					if(i>=commandnum){
						break;
					}
				}
			}
			if(flag2==1){
				flag2=0;
			}
			else if(strcmp(tempcommand[0],"cd")==0){
				status = chdir(tempcommand[1]);
				if(status==-1)
					ret = 1;
				else 
					ret = 0;
			}
			else if(strcmp(tempcommand[0],"pwd")==0){
				printf("%s\n",cwd);
				ret = 0;
			}
			else if(strcmp(tempcommand[0],"clear")==0){
				clear();
				ret = 0;
			}
			else{
				ret = runcommand(num,flag1,&i);
				flag1=0;
			}
			if(i<commandnum)
				decode(i,ret,&flag1,&flag2);//previous seperator.
		}
	}
	return 0;
}

int breakcommand(char *str){
	int len = strlen(str);
	int commandnum=0,index=0,flag=0;
	memset(command,'\0',50*100*sizeof(char));
	for(int i=0;i<len;i++){
		if(str[i]==' '||str[i]=='\t' ){
			if(flag!=1){
				flag=1;
				command[commandnum][index] = '\0';
				commandnum++;
				index=0;
			}
			continue;
		}
		else if(str[i]==';'||str[i]=='>'||str[i]=='<'||(str[i]=='&'&&str[i+1]!='&')||(str[i]=='|'&&str[i+1]!='|')){
			if(flag!=1){
				flag=1;
				command[commandnum][index] = '\0';
				commandnum++;
				index=0;
				command[commandnum][index] = str[i];
				command[commandnum][index+1] = '\0';
				commandnum++;
			}
			else{
				command[commandnum][index] = str[i];
				command[commandnum][index+1] = '\0';
				commandnum++;
			}
		}
		else if((str[i]=='&'&&str[i+1]=='&')||(str[i]=='|'&&str[i+1]=='|')){
			if(flag!=1){
				flag=1;
				command[commandnum][index] = '\0';
				commandnum++;
				index=0;
				command[commandnum][index] = str[i];
				command[commandnum][index+1] = str[i+1];
				command[commandnum][index+2] = '\0';
				commandnum++;
				i++;
			}
			else{
				command[commandnum][index] = str[i];
				command[commandnum][index+1] = str[i+1];
				command[commandnum][index+2] = '\0';
				commandnum++;
				i++;
			}
		}
		else if(str[i]=='\n'){
			command[commandnum][index] = '\0';
			commandnum++;
			break;
		}
		else{
			command[commandnum][index]=str[i];
			index++;
			flag = 0;
		}
	}
	return commandnum;
}

void decode(int i,int ret,int *flag1,int *flag2){
    if(strcmp(command[i],"&&")==0||strcmp(command[i-1],"&&")==0){
		if(ret!=0){
			//Not Successful.
			*flag1 = 2;
			*flag2 = 1;
		}
		else{
			//Succesful.
			*flag1 = 2;
			*flag2 = 0;
		}
	}
	else if(strcmp(command[i],"||")==0||strcmp(command[i-1],"||")==0){
		if(ret!=0){
			//Not Successful.
			*flag1 = 4;
			*flag2 = 0;
		}
		else {
			//Successful.
			*flag1 = 4;
			*flag2 = 1;
		}
	}
	else if(strcmp(command[i],";")==0||strcmp(command[i-1],";")==0){
		*flag1 = 5;
		*flag2 = 0;
	}
	else if(strcmp(command[i],"|")==0||strcmp(command[i-1],"|")==0){
		*flag1 = 8;
		*flag2 = 0;
	}
}
//Flag1----/&---1/&&---2/|---3/||---4/;---5/>---6/<---7/----------FLAG VALUES
//Flag2----Status.
int runcommand(int num,int flag,int *i){
	pid_t p;
	int status,fd;
	char *str[10];
	for(int i=0;i<num;i++){
		str[i] = (char *)malloc(20*sizeof(char));
		strcpy(str[i],tempcommand[i]);
	}
	str[num] = NULL;
	if(flag==3){
		pipe(pd1);
		pipecount++;
	}
	else if(flag==9){
		if(pipecount%2==1)
			pipe(pd2);
		else
			pipe(pd1);
		pipecount++;

	}
	else if(flag==6||flag==7){
		if(flag==6){
			fd = open(command[*i+1],O_CREAT|O_WRONLY,0666);
			lseek(fd,0,SEEK_END);
		}
		else{
			fd = open(command[*i+1],O_RDONLY);
			if(fd==-1){
				printf("No such file in Directory.\n");
				return 1;
			}
			lseek(fd,0,SEEK_SET);
		}
		*i+=2;	
	}
	p=fork();
	if(p==0){
		if(flag==3)
			dup2(pd1[1],1);
		else if(flag==9){
			if(pipecount%2==0){
				dup2(pd1[0],0);
				dup2(pd2[1],1);
			}
			else{
				dup2(pd2[0],0);
				dup2(pd1[1],1);
			}
		}
		else if(flag==8)
			if(pipecount%2==0)
				dup2(pd2[0],0);
			else
				dup2(pd1[0],0);
		else if(flag==6)
			dup2(fd,1);
		else if(flag==7)
			dup2(fd,0);
		status = execvp(tempcommand[0],str);
		if(status ==-1){
			perror("execvp");
		}
		exit(1);
	}
	else{
		if(flag==1)
			waitpid(p,&status,0|WNOHANG|WNOWAIT);
		else
			wait(&status);
		if(flag==3)
			close(pd1[1]);
		else if(flag==9){
			if(pipecount%2==0){
				close(pd1[0]);
				close(pd2[1]);
			}
			else{
				close(pd2[0]);
				close(pd2[1]);
			}

		}
		else if(flag==8){
			if(pipecount%2==0)
				close(pd2[0]);
			else
				close(pd1[0]);
		}
		else if(flag==6||flag==7)
			close(fd);
	}
	return status;	
}
int fileexec(int num,const char **str){
	for(int x=1;x<=num;x++){
		int status; 
		int ret;
		FILE *fp = fopen(str[x],"r");
		if(fp==NULL){
			perror("fopen");
			return -1;
		}
		while(1){
			wait(&status);
			int len;
			char cwd[1000];
			char input[100];
			getcwd(cwd,sizeof(cwd));
			if(fgets(input,100,fp)==NULL){
				fclose(fp);
				break;
			}
			if(strcmp(input,"\n")==0)
				continue;
			if(strcmp(input,"exit\n")==0)
				break;
			commandnum = breakcommand(input);
			int flag1=0,flag2=0;
			for(int i=0;i<commandnum;i++){
				memset(tempcommand,'\0',sizeof(char)*50*100);
				int num = 0;
				for(int j=0;;j++){
					if(strcmp(command[i],"&")==0||strcmp(command[i],"|")==0||strcmp(command[i],"&&")==0||strcmp(command[i],"||")==0||strcmp(command[i],";")==0||strcmp(command[i],">")==0||strcmp(command[i],"<")==0){
						if(strcmp(command[i],"&")==0)
							flag1 = 1;
						else if(strcmp(command[i],"|")==0)
							flag1 = 3;
						else if(strcmp(command[i],">")==0)
							flag1 = 6;
						
						else if(strcmp(command[i],"<")==0)
							flag1 = 7;
						break;
					}
					else{
						strcpy(tempcommand[j],command[i]);
						i++;
						num++;
						if(i>=commandnum){
							break;
						}
					}
				}
				if(flag2==1){
					flag2=0;
				}
				else if(strcmp(tempcommand[0],"cd")==0){
					status = chdir(tempcommand[1]);
					if(status==-1)
						ret = 1;
					else 
						ret = 0;
				}
				else if(strcmp(tempcommand[0],"pwd")==0){
					printf("%s\n",cwd);
					ret = 0;
				}
				else if(strcmp(tempcommand[0],"clear")==0){
					clear();
					ret = 0;
				}
				else{
					ret = runcommand(num,flag1,&i);
					flag1=0;
				}
				if(i<commandnum)
					decode(i,ret,&flag1,&flag2);
			}
			
		}
		fclose(fp);
	}
}

