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
#include <time.h>
#include <sys/time.h>

//GLOBAL VARIABLE
struct sockaddr_storage their_addr;
socklen_t addr_size;
struct addrinfo hints, *res;
int sockfd,ret;
int sum=0;
long start,end,rtt;
char buffer[1400];
char temp[20];
char packet[1400] ="";

uint32_t tempts,tempseq;

long getMicrotime(){
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
}

int main(int argc, char const *argv[])
{
	int seq,ttl,numP,ttlnew;
	if(argc!=6){
		printf("Usage:executable IP PortNo Payload TTL NumPackets\n");
		exit(1);
	}
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	getaddrinfo(argv[1],argv[2],&hints,&res);
	sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);

	numP = atoi(argv[5]);
	int payload = atoi(argv[3]);
	seq = 0;
	int i=0;
	for(i=1;i<=numP;i++){
		ttl = atoi(argv[4]);
		
		while(1){
			//Sequence Number
			seq = i;
			tempseq = htonl(seq);
			memcpy(&packet[0],&tempseq,2);
			//Time
			start = getMicrotime();
			tempts = htonl(start);
			memcpy(&packet[2],&tempts,4);
			//TTL
			memcpy(&packet[6],&ttl,1);
			ret = sendto(sockfd,packet,(payload+7),0,res->ai_addr,res->ai_addrlen);
			addr_size = sizeof their_addr;
			memset(buffer,'\0',sizeof buffer);	
			ret = recvfrom(sockfd,buffer,1307,0,(struct sockaddr *)&their_addr,&addr_size);
			ttlnew = (int)buffer[6];
			if(ttlnew==ttl){
				printf("Packet didn't reach the server side.\n");
				break;
			}
			else 
				ttl = ttlnew;
			ttl--;
			if(ttl==0) break;
			//memcpy(&packet[6],&ttl,1);
		}
		end = getMicrotime();
		memcpy(&tempts,&packet[2],4);
		start = ntohl(tempts);
		rtt = (end - start);
		sum = sum + rtt;
	}
	double avg = (double)sum/numP;
	printf("%lf\n",avg);
	FILE *fp;
	fp = fopen("res.txt","a");
	fprintf(fp,"\nFor TTL = %s\n", argv[4]);
	fprintf(fp,"%s,%lf\n",argv[3],avg);
	fprintf(fp,"******************\n"); 
	fclose(fp);
	return 0;
}
