// #define _POSIX_C_SOURCE 200112L
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

//GLOBAL VAIABLE

	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	struct addrinfo hints, *res;
	int sockfd , newfd;
	char buffer[100];
	pthread_t tid1,tid2;

void* sendfun(){
	while(1){
		char str[100];
		fgets(str,100,stdin);
		strtok(str, "\n");

		send(newfd,str,strlen(str),0);
		
		if(strcmp(str,"quit")==0)
			exit(0);

		memset(str, '\0', strlen(str));
	}
	pthread_exit(0);
}

void* recefun(){
	while(1){
		int val = recv(newfd,buffer,(100),0);
		if(val<=0)
			break;
		if(strcmp(buffer,"quit")==0)
			exit(0);
		printf("c:%s\n",buffer);
		memset(buffer, '\0', strlen(buffer));
	}
	pthread_exit(0);
}

int main(int argc, char const *argv[])
{
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL,"3490",&hints,&res);
	sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	bind(sockfd,res->ai_addr,res->ai_addrlen);
	listen(sockfd,1);
	addr_size = sizeof their_addr;
	printf("Waiting for Connection.\n");
	newfd = accept(sockfd,(struct sockaddr *)&their_addr,&addr_size);
	printf("Type 'quit' to Exit.\n");

	pthread_create(&tid1,NULL,sendfun,NULL);
	pthread_create(&tid2,NULL,recefun,NULL);

	close(sockfd);
	// return 0;
	pthread_exit(0);

}
