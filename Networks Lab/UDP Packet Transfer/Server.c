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
#include <sys/time.h>

struct sockaddr_storage their_addr;
socklen_t addr_size;
struct addrinfo hints, *res;
int sockfd , newfd,ret;

int main(int argc, char const *argv[])
{
	char buffer[1400];
	char serverPort[10];
	char temp[10];
	int ttl;
	memset(&hints, 0 , sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL,argv[1],&hints,&res);
	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	bind(sockfd,res->ai_addr,res->ai_addrlen);
	addr_size = sizeof their_addr;
	while(1){
		ret = recvfrom(sockfd,buffer,1390,0,(struct sockaddr *)&their_addr,&addr_size);
		ttl = (int)buffer[6];
		ttl--;
		memcpy(&buffer[6],&ttl,1);
		ret = sendto(sockfd,buffer,sizeof(buffer),0,(struct sockaddr *)&their_addr,addr_size);
		memset(buffer,'\0',sizeof buffer);
	 }
	return 0;
}
