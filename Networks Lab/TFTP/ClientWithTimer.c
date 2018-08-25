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
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

#define PORT "69"
#define RETRY 10
char* mode = "octet";
struct sockaddr_storage their_addr;
socklen_t addr_size;
struct addrinfo hints, *res;
int sockfd,ret;
uint16_t temp;

void promt();
void get(char *);
void put(char *);
int recvfromTimeOut(long,long);

int main(int argc, char const *argv[])
{	
	if(argc<2){
		printf("Usage:./tftp <address> \n");
		return 1;
	}
	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	getaddrinfo(argv[1],PORT,&hints,&res);
	sockfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
	// freeaddrinfo(&hints);
	promt();
	return 0;
}

void promt(){
	char ptr1[10];
	char ptr2[10];
	while(1){
		printf("tftp>");
		scanf("%s",ptr1);
		if(strcmp(ptr1,"get")==0){
			scanf("%s",ptr2);
			fcntl(sockfd,F_SETFL,O_NONBLOCK);
			get(ptr2);
		}
		else if(strcmp(ptr1,"put")==0){
			scanf("%s",ptr2);
			fcntl(sockfd,F_SETFL,O_NONBLOCK);
			put(ptr2);
		}
		else if(strcmp(ptr1,"exit")==0){
			exit(0);
		}
		else
			printf("Bad Command.\n");
	}
}

void get(char *file){
	int len = strlen(file);
	int ret;
	unsigned char rrq[len+9],ack[4],data[516];
	int fd = open(file,O_WRONLY|O_CREAT,0666);
	//RRQ Packet Formation.
	rrq[0]=0;
	rrq[1]=1;
	strcpy(&rrq[2],file);
	rrq[len+2] = 0;
	strcpy(&rrq[len+3],mode);
	rrq[len+8] = 0;
	ret = sendto(sockfd,rrq,len+9,0,res->ai_addr,res->ai_addrlen);
	if(ret==-1){
		printf("Error Sending.\n");
		return;
	}
	printf("Read Request Send.\n");
	int block_no = 0,flag=0,total=0,retry = RETRY,timeres,isretransmit,bytes;
	while(1){
		if(isretransmit==1&&block_no == 1&&retry>0)
			sendto(sockfd,rrq,len+9,0,res->ai_addr,res->ai_addrlen);
		else if(isretransmit==1&&block_no>1&&retry>0)
			sendto(sockfd,ack,4,0,(struct sockaddr *)&their_addr,addr_size);
		else if(retry == 0){
			printf("Max retry Reached.\n");
			exit(-1);
		}
		timeres = recvfromTimeOut(1,0);
		switch(timeres){
			case 0:
					printf("Time Out---Retries left = %d.\n",(--retry));
					isretransmit = 1;
					break;
			case -1:
					printf("Error with Timer recvfromTimeOut()\n");
					exit(1);
					break;
			default:
					addr_size = sizeof(their_addr);
					memset(data,0,516);
					ret = recvfrom(sockfd,data,516,0,(struct sockaddr *)&their_addr,&addr_size);
					if(ret == -1)
						perror("recvfrom");
					else{
						// memcpy(&temp,data,2);
						total+=ret;
						int op_code = data[1];
						if(op_code==5){
							printf("ERROR.\n");
							return;
						}
						else if(op_code==3){
							if((ret-4)!=512){
								flag = 1;
							}
							write(fd,&data[4],ret-4);
							block_no++;
							//ACK Formation.
							ack[0] = 0;
							ack[1] = 4;
							ack[2] = block_no/256;
							ack[3] = block_no%256;
	 						ret = sendto(sockfd,ack,4,0,(struct sockaddr *)&their_addr,addr_size);
							if(ret==-1){  
								printf("Error Sending Acknowledgement.\n");
							}
						}
					}
		}
		if(flag==1)
			break;
	}
	close(fd);
	printf("Data Transfer Complete.\n");
	printf("Received %d bytes.\n",total);

}

void put(char *file){
	int len = strlen(file);
	int ret;
	unsigned char wrq[len+9],ack[4],data[516];
	int fd = open(file,O_RDONLY);
	if(fd==-1){
		printf("Bad File Name.\n");
		return;
	}
	//WRQ Packet Formation.
	wrq[0] = 0;
	wrq[1] = 2;
	strcpy(&wrq[2],file);
	wrq[2+len] = 0;
	strcpy(&wrq[len+3],mode);
	wrq[8+len] = 0;
	ret = sendto(sockfd,wrq,len+9,0,res->ai_addr,res->ai_addrlen);
	if(ret==-1){
		printf("Error Sending.\n");
		return;
	}
	printf("Write Request Send\n");
	int block_no = 0,flag=0,total=0,retry = RETRY,timeres,isretransmit,bytes;
	while(1){
		if(isretransmit==1&&block_no == 0&&retry>0)
			sendto(sockfd,wrq,len+9,0,res->ai_addr,res->ai_addrlen);
		else if(isretransmit==1&&block_no>0&&retry>0)
			sendto(sockfd,data,bytes+4,0,(struct sockaddr *)&their_addr,addr_size);
		if(retry == 0){
			printf("Max retry Reached.\n");
			return;
		}
		timeres = recvfromTimeOut(1,0);
		switch(timeres){
			case 0:
					printf("Time Out---Retries left = %d.\n",(--retry));
					isretransmit = 1;
					break;
			case -1:
					printf("Error with Timer recvfromTimeOut()\n");
					exit(1);
					break;
			default:
					addr_size = sizeof(their_addr);
					isretransmit = 0;
					ret = recvfrom(sockfd,ack,4,0,(struct sockaddr *)&their_addr,&addr_size);
					if(ret == -1)
						perror("recvfrom");
					else{
						int op_code = ack[1];
						if(op_code==5){
							printf("ERROR.\n");
							return;
						}
						else if(op_code==4){
							if(flag==1){
								flag=2;
								break;
							}
							//Data packet Formation.
							block_no++;
							memset(data,0,516);
							data[0] = 0;
							data[1] = 3;
							data[2] = block_no/256;
							data[3] = block_no%256;
							bytes = read(fd,data+4,512);
							total+=bytes;
							ret = sendto(sockfd,data,bytes+4,0,(struct sockaddr *)&their_addr,addr_size);
							if(ret==-1){
								printf("Error Sending Data.\n");
							}
							retry=RETRY;
							if(bytes<512)
								flag=1;
						}
					}
		}
		if(flag==2)
			break;
	}
	close(fd);
	printf("Data Transfer Complete.\n");
	printf("Send %d bytes.\n",total);
}

int recvfromTimeOut(long sec,long usec){
	struct timeval timeout;
	timeout.tv_sec = sec;
	timeout.tv_usec = usec;
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(sockfd,&fds);
	return select(FD_SETSIZE,&fds,NULL,NULL,&timeout);
}