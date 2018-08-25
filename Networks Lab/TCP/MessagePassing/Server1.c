//#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
 #include <unistd.h>

int main(int argc, char const *argv[])
{
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	struct addrinfo hints, *res;
	int sockfd , newfd;
	char buffer[1024] = {0};
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL,"3490",&hints,&res);
	sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	bind(sockfd,res->ai_addr,res->ai_addrlen);
	listen(sockfd,1);
	addr_size = sizeof their_addr;
	newfd = accept(sockfd,(struct sockaddr *)&their_addr,&addr_size);
	int val = recv(newfd,buffer,1024,0);
	printf("%s\n",buffer);
	send(newfd,buffer,strlen(buffer),0);
	printf("Message Sent to Client");
	close(sockfd);
	return 0;
}
