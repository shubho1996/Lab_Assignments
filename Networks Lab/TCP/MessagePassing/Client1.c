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
	struct addrinfo hints, *res;
	int sockfd;
	char buffer[1024] = {0};
	char *str = malloc(sizeof(char)*100);
	fgets(str,100,stdin);
	strtok(str, "\n");
	
	memset(&hints, 0 , sizeof(hints));
	
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	

	getaddrinfo("127.0.0.1","3490",&hints,&res);

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	connect(sockfd, res->ai_addr, res->ai_addrlen);

	send(sockfd,str,strlen(str),0);
	printf("Message Sent to Server.\n");

	int val = recv(sockfd,buffer,1024,0);

	printf("%s\n",buffer);

	close(sockfd);

	return 0;
}