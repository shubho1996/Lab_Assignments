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

typedef struct client_info
{
	char Name[100];
	int sock;
	char message[100];
	char Destination[100];
}client;

struct sockaddr_storage their_addr;
socklen_t addr_size;
struct addrinfo hints, *res;
int sockfd,count;
client arr[10];
pthread_t threadarr[10],mainthead;


void sendfun(client *data){
	
	client *ptr = (client *)data;	
	char reply[100] = "";
	strcat(reply,ptr->Name);
	strcat(reply,":");
	strcat(reply,ptr->message);
	for (int j = 0; j < count; ++j){

		if(strcmp(arr[j].Name,ptr->Destination)==0){
			send(arr[j].sock,reply,strlen(reply),0);
			break;
		}
	}
	memset(ptr->message, '\0', strlen(ptr->message));
	memset(reply,'\0',strlen(reply));

}


void* recefun(void *data){
	while(1){
		client *ptr = (client *)data;
		int val = recv(ptr->sock,ptr->message,100,0);
		if(val<=0)
			break;
		if(strcmp(ptr->message,"quit")==0)
			break;
		sendfun(ptr);
	}
	pthread_exit(0);
}

int main(int argc, char const *argv[])
{
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL,"3490",&hints,&res);
	sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	bind(sockfd,res->ai_addr,res->ai_addrlen);
	listen(sockfd,10);
	addr_size = sizeof their_addr;

	count = 0;
	for(int i=0;i<10;i++){
		printf("Waiting for Connection.\n");
		arr[i].sock = accept(sockfd,(struct sockaddr *)&their_addr,&addr_size);
		int valread1 = recv(arr[i].sock,arr[i].Name,100,0);
		int valread2 = recv(arr[i].sock,arr[i].Destination,100,0);
		printf("Client Connected %s\n",arr[i].Name);
		pthread_create(&threadarr[i],NULL,recefun,&arr[i]);
		count++;
	}
	pthread_exit(0);
}
