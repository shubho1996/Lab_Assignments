#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

//Global Variables
struct addrinfo hints, *res;
int sockfd;
char buffer[100]={0};
pthread_t tid1,tid2;

void* sendfun(){
	while(1){
		char str[100];
		fgets(str,100,stdin);
		strtok(str, "\n");

		send(sockfd,str,strlen(str),0);	

		if(strcmp(str,"quit")==0)
			exit(0);

		memset(str, '\0', strlen(str));
	}
	pthread_exit(0);
}

void* recefun(){
	while(1){
		int val = recv(sockfd,buffer,(100),0);

		if(val <= 0){
			break;
		 }

		if(strcmp(buffer,"quit")==0)
			exit(0);

		printf("s:%s\n",buffer);
		memset(buffer, '\0', strlen(buffer));
	}
	close(sockfd);
	pthread_exit(0);
}

int main(int argc, char const *argv[])
{
	
	// char buffer[1024] = {0};
	// char *str = malloc(sizeof(char)*100);
	// fgets(str,100,stdin);
	
	memset(&hints, 0 , sizeof(hints));
	
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	

	getaddrinfo("127.0.0.1","3490",&hints,&res);

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	connect(sockfd , res->ai_addr, res->ai_addrlen);
	//perror("client:connect");
	printf("Type quit for Exit!\n");

	pthread_create(&tid1,NULL,sendfun,NULL);
	pthread_create(&tid2,NULL,recefun,NULL);

	
	//close(sockfd);
	pthread_exit(0);

	// return 0;
}