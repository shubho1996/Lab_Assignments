/*Flat file system implementation.*/
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

#define INODEBM 2000	//Inode Bit Map Size
#define DNODEBM 20000	//Data Node Bit Map Size
#define MAXDRIVE 10	
#define INODETABLE 20	//Max Entries in a particular Inode.
#define INVALID 4294967295
#define SB_SIZE sizeof(superblock)
#define IN_SIZE sizeof(inode)
#define MAXFILE 10		

typedef struct{
	unsigned long sizeOfFs;			
	unsigned int blockSize;
	unsigned int iNodeStart;
	unsigned int iNodesize;
	unsigned int noOfInode;
	unsigned int noOfFreeInode;		//CHANGES....
	unsigned int dataNodeStart;
	unsigned int noOfdataNode;
	unsigned int noOfFreeDataNode;	//CHANGES....
	unsigned int rootNodeNo;
	unsigned char IBM[INODEBM];		//CHANGES....
	unsigned char DBM[DNODEBM];		//CHANGES....

}superblock;

typedef struct{
	char name[20];
	unsigned int size;	//File - Size in Bytes and Root - No. of Entries.
	unsigned int dataBlocks;	//1 for root
	unsigned int table[INODETABLE];	//only one entry for root node.
}inode;

typedef struct{
	char nameFS[20];
	char drive;
	bool status;
	int fd;
	superblock sbvar;
	inode rootnodevar;
}mount;

typedef struct{
	char filename[20];
	unsigned int size;
	unsigned int inodelocation;
}filenode;

typedef struct{
	char drive;
	char filename[20];
}decodestruct;

mount mountdesc[MAXDRIVE];

int createfs(char *,char *,char *);
int mountfun(char *,char *);
int unmount(char *);
int copy(char ,char *,char *);
int ls(char );
int delete(char ,char *);
int move(char ,char ,char *,char *);
unsigned long getsize(char *);
unsigned long fsize(char* );
decodestruct decode(char *);
void decitobi(int *,unsigned char );
unsigned char bitodeci(int *);
unsigned int  getpos(char );
void help();
void mounteddrive();


int main(int argc,char const *argv[]){

	for(int i=0;i<MAXDRIVE;i++){
		mountdesc[i].status=false;
	}
	printf("Type \"help\" to know the syntax of Commands.\n");
	printf("Type \"exit\" to exit the program.\n");
	//promt.
	while(1){
		printf("myfs>");
		char *ptr = malloc(10*sizeof(char));
		char *ptr1 = malloc(10*sizeof(char));
		char *ptr2 = malloc(10*sizeof(char));
		char *ptr3 = malloc(10*sizeof(char));
		//Getting the User Input.
		scanf("%s",ptr);
		if(strcmp(ptr,"mkfs")==0){
			//File system Name
			scanf("%s",ptr1);
			//blocksize
			scanf("%s",ptr2);
			//total size
			scanf("%s",ptr3);
			createfs(ptr1,ptr2,ptr3);
		}
		else if(strcmp(ptr,"mount")==0){
			//Name of File System.
			scanf("%s",ptr1);
			//Drive Name
			scanf("%s",ptr2);
			mountfun(ptr1,ptr2);
		}
		else if(strcmp(ptr,"unmount")==0){
			scanf("%s",ptr1);
			unmount(ptr1);
		}
		else if(strcmp(ptr,"cp")==0){
			scanf("%s %s",ptr1,ptr2);
			decodestruct temp;
			temp = decode(ptr2);
			copy(temp.drive,ptr1,temp.filename);
		}
		else if(strcmp(ptr,"ls")==0){
			scanf("%s",ptr1);
			ls(ptr1[0]);
		}
		else if(strcmp(ptr,"delete")==0){
			scanf("%s",ptr1);
			decodestruct temp;
			temp = decode(ptr1);
			delete(temp.drive,temp.filename);
		}
		else if(strcmp(ptr,"move")==0){
			scanf("%s %s",ptr1,ptr2);
			decodestruct temp1,temp2;
			temp1 = decode(ptr1);
			temp2 = decode(ptr2);
			move(temp1.drive,temp2.drive,temp1.filename,temp2.filename);

		}
		else if(strcmp(ptr,"help")==0){
			help();
		}
		else if(strcmp(ptr,"list")==0){
			scanf("%s",ptr1);
			if(strcmp(ptr1,"drives")==0){
				mounteddrive();
			}
			else
				printf("Unknown Command.\n");
		}
		else if(strcmp(ptr,"exit")==0)
			exit(0);
		else
			printf("Unknown Command.\n");

	}


	return 0;
}

//Create file system.
int createfs(char ptr1[],char ptr2[],char ptr3[]){

	superblock sbvar;
	sbvar.sizeOfFs = getsize(ptr3);
	sbvar.blockSize = atoi(ptr2);
	sbvar.iNodeStart = SB_SIZE;
	sbvar.iNodesize = IN_SIZE;
	sbvar.noOfInode = (0.2*(sbvar.sizeOfFs-SB_SIZE))/IN_SIZE; 				//20% - space for inode blocks.
	sbvar.noOfFreeInode = sbvar.noOfInode-1;
	sbvar.dataNodeStart = SB_SIZE+(0.2*(sbvar.sizeOfFs-SB_SIZE));
	sbvar.noOfdataNode = (0.8*(sbvar.sizeOfFs-SB_SIZE))/sbvar.blockSize;	//80% - space for data blocks.
	sbvar.noOfFreeDataNode = sbvar.noOfdataNode-1;
	sbvar.rootNodeNo = 0;
	for(int i=0;i<INODEBM;i++){
		sbvar.IBM[i]=0;
	}
	for(int i=0;i<DNODEBM;i++){
		sbvar.DBM[i]=0;
	}
	sbvar.DBM[0]=128;
	sbvar.IBM[0]=128;

	int fd;
	//int open(const char *pathname, int flags, mode_t mode);
	fd = open(ptr1,O_CREAT|O_RDWR,S_IRWXU);
	//Changing the size of file system.
	off_t size =(off_t ) sbvar.sizeOfFs;
	// int ftruncate(int fd, off_t length);
	ftruncate(fd,size);
	//ssize_t write(int fd, const void *buf, size_t count);
	write(fd,&sbvar,SB_SIZE);

	//setting the root node variable.
	inode rootnodevar;
	strcpy(rootnodevar.name,"root");
	rootnodevar.size = 0;
	rootnodevar.dataBlocks = 1;
	for(int i=0;i<INODETABLE;i++){
		rootnodevar.table[i] = INVALID;
	}
	rootnodevar.table[0] = 0;
	// off_t lseek(int fd, off_t offset, int whence);
	lseek(fd,(sizeof(superblock)),SEEK_SET);
	write(fd,&rootnodevar,sizeof(rootnodevar));
	close(fd);

}

//Storing the file system to a drive.
int mountfun(char *ptr1,char *ptr2){

	for(int i=0;i<MAXDRIVE;i++){
		if(mountdesc[i].status==false){
			strcpy(mountdesc[i].nameFS,ptr1);
			mountdesc[i].drive=ptr2[0];
			mountdesc[i].status=true;
			mountdesc[i].fd = open(ptr1,O_CREAT|O_RDWR,S_IRWXU);
			read(mountdesc[i].fd,&(mountdesc[i].sbvar),SB_SIZE);
			read(mountdesc[i].fd,&(mountdesc[i].rootnodevar),IN_SIZE);
			break;
		}
	}
	
	return 0;
}


int unmount(char *ptr1){
	for(int i=0;i<MAXDRIVE;i++){
		if(mountdesc[i].status==true){
			if(ptr1[0]==mountdesc[i].drive){
				mountdesc[i].status = false;
				close(mountdesc[i].fd);
				break;
			}
		}
	}
	return 0;
}


int copy(char Drive,char *oldfilename,char *newfilename){

	int fd;
	fd = open(oldfilename,O_CREAT|O_RDWR,S_IRWXU);

	unsigned int pos = getpos(Drive);
	//Getting the requirement.
	unsigned int datablockreq;
	datablockreq = fsize(oldfilename)/mountdesc[pos].sbvar.blockSize;
	if(datablockreq>INODETABLE)
		return -1;

	unsigned int datapos[datablockreq];
	unsigned int inodepos;
	int flag=0;
	//Getting the free Inode block.
 	for(unsigned int i=0;i<mountdesc[pos].sbvar.noOfInode;i++){
 		if(mountdesc[pos].sbvar.IBM[i]<255){
 			unsigned char num = mountdesc[pos].sbvar.IBM[i];
 			int arr[8];
 			decitobi(arr,num);
 			for(int x=7;x>=0;x--){
 				if(arr[x]==0){
 					inodepos = i*8+x;
 					arr[x]=1;
 					num = bitodeci(arr);
 					mountdesc[pos].sbvar.IBM[i]=num;
 					flag = 1;
 					break;
 				}
 			}
 			break;
 		}
 	}
 	//Getting the free Data Block.
 	for(int j=0;j<datablockreq;j++){
 		for(unsigned int i=0;i<mountdesc[pos].sbvar.noOfdataNode;i++){
 			if(mountdesc[pos].sbvar.DBM[i]<255){
 				unsigned char num = mountdesc[pos].sbvar.DBM[i];
 				int arr[8];
 				decitobi(arr,num);
 				for(int x=7;x>=0;x--){
 					if(arr[x]==0){
 						datapos[j] = i*8+x;
 						arr[x]=1;
 						num = bitodeci(arr);
 						mountdesc[pos].sbvar.DBM[i]=num;
 						flag = 1;
 						break;
 					}
 				}
 				break;
 			}
 		}
 	}
 	//Setting the meta data for the file.
 	inode invar;
 	strcpy(invar.name,newfilename);
 	strcat(invar.name,"\0");
 	invar.size = fsize(oldfilename);
 	invar.dataBlocks = datablockreq;
 	for(int i=0;i<datablockreq;i++){
 		invar.table[i] = datapos[i];
 	}
 	//setting the remaining inode node table entry to Invalid.
 	if(datablockreq<INODETABLE){
 		for(int i=datablockreq;i<INODETABLE;i++)
 			invar.table[i] = INVALID;

 	}
 	lseek(mountdesc[pos].fd,(SB_SIZE+(inodepos*IN_SIZE)),SEEK_SET);
 	write(mountdesc[pos].fd,&invar,IN_SIZE);
 	//writing the file to the data node.
 	void *buffer;
 	for(int i=0;i<datablockreq;i++){
 		lseek(fd,(i*mountdesc[pos].sbvar.blockSize),SEEK_SET);
 		lseek(mountdesc[pos].fd,((datapos[i]*mountdesc[pos].sbvar.blockSize)+SB_SIZE),SEEK_SET);
 		read(fd,buffer,mountdesc[pos].sbvar.blockSize);
 		write(mountdesc[pos].fd,buffer,mountdesc[pos].sbvar.blockSize);
 	}
 	//Writing the entry in the root node.
 	filenode temp;
 	strcpy(temp.filename,newfilename);
 	strcat(temp.filename,"\0");
 	temp.inodelocation = inodepos;
 	temp.size = fsize(oldfilename);	
 	lseek(mountdesc[pos].fd,(mountdesc[pos].sbvar.dataNodeStart+(mountdesc[pos].rootnodevar.size*sizeof(temp))),SEEK_SET);
 	write(mountdesc[pos].fd,&temp,sizeof(temp));

 	//Updating the superblock and root node metadata.
 	mountdesc[pos].rootnodevar.size++;
 	mountdesc[pos].sbvar.noOfFreeInode--;
 	mountdesc[pos].sbvar.noOfFreeDataNode--;

 	lseek(mountdesc[pos].fd,0,SEEK_SET);
 	write(mountdesc[pos].fd,&(mountdesc[pos].sbvar),SB_SIZE);
 	write(mountdesc[pos].fd,&(mountdesc[pos].rootnodevar),IN_SIZE);

 	return 0;
}

//List the files in a Drive.
int ls(char Drive){

	unsigned int pos = getpos(Drive);
	//Getting the list of file.
	filenode fnode[20];
	lseek(mountdesc[pos].fd,mountdesc[pos].sbvar.dataNodeStart,SEEK_SET);
	for(int i=0;i<mountdesc[pos].rootnodevar.size;i++){
		read(mountdesc[pos].fd,&fnode[i],sizeof(filenode));
	}
	//Printing the Files.
	for(int i=0;i<mountdesc[pos].rootnodevar.size;i++){
		printf("%s ---- %u Bytes\n",fnode[i].filename,fnode[i].size);
	}

}

//Delete a file from Drive.
int delete(char Drive,char *filename){
	
	unsigned int pos = getpos(Drive);
	//Getting the list of file from rootnode datablock.
	filenode fnode[20];
	lseek(mountdesc[pos].fd,mountdesc[pos].sbvar.dataNodeStart,SEEK_SET);
	for(int i=0;i<mountdesc[pos].rootnodevar.size;i++){
		read(mountdesc[pos].fd,&fnode[i],sizeof(filenode));
	}

	//Checking the position of the filename.
	unsigned int location,filepos;
	for(int i=0;i<mountdesc[pos].rootnodevar.size;i++){
		if(strcmp(fnode[i].filename,filename)==0){
			location = fnode[i].inodelocation;
			filepos = i;
			break;
		}
	}
	//Getting the Inode of the file.
	inode invar;
	lseek(mountdesc[pos].fd,mountdesc[pos].sbvar.iNodeStart+(location*IN_SIZE),SEEK_SET);
	read(mountdesc[pos].fd,&invar,IN_SIZE);
	//Free the Data nodes associated with the file.
	for(int i=0;;i++){
		if(invar.table[i]==INVALID)
			break;
		unsigned int temp1,temp2,arr[8];
		unsigned char temp;
		//Element of the DBM[].
		temp1 = invar.table[i]/8;
		//Freeing the Bit position. 
		temp2 = 7-(invar.table[i]%8);
		temp = mountdesc[pos].sbvar.DBM[temp1];
		decitobi(arr,temp);
		arr[temp2]=0;
		temp = bitodeci(arr);
		mountdesc[pos].sbvar.DBM[temp1] = temp;
		mountdesc[pos].sbvar.noOfFreeDataNode--;
	}
	//Free the Inode associated with the file.
	unsigned int temp1,temp2,arr[8];
	unsigned char temp;
	temp1 = location/8;
	temp2 = 7-(location%8);
	temp = mountdesc[pos].sbvar.IBM[temp1];
	decitobi(arr,temp);
	arr[temp2]=0;
	temp = bitodeci(arr);
	mountdesc[pos].sbvar.IBM[temp1] = temp;
	mountdesc[pos].sbvar.noOfFreeInode--;
	mountdesc[pos].rootnodevar.size--;
	//writing back the elements to the file system.
	lseek(mountdesc[pos].fd,0,SEEK_SET);
	write(mountdesc[pos].fd,&(mountdesc[pos].sbvar),SB_SIZE);
	lseek(mountdesc[pos].fd,mountdesc[pos].sbvar.iNodeStart,SEEK_SET);
	write(mountdesc[pos].fd,&(mountdesc[pos].rootnodevar),IN_SIZE);

	lseek(mountdesc[pos].fd,mountdesc[pos].sbvar.dataNodeStart+(filepos*sizeof(filenode)),SEEK_SET);
	for(int i=filepos;i<mountdesc[pos].rootnodevar.size;i++){
		write(mountdesc[pos].fd,&fnode[i+1],sizeof(filenode));
	}

	return 0;
}

//Moving a File from one drive to another file.
int move(char srcdrive,char destdrive,char *oldfile,char *newfile){

	unsigned int pos = getpos(srcdrive);
	//Opening a temporary file.
	int fd = open("$Cache$",O_CREAT|O_RDWR,S_IRWXU);
	//Getting the list of files.
	void *buffer;
	filenode fnode[20];
	lseek(mountdesc[pos].fd,mountdesc[pos].sbvar.dataNodeStart,SEEK_SET);
	for(int i=0;i<mountdesc[pos].rootnodevar.size;i++){
		read(mountdesc[pos].fd,&fnode[i],sizeof(filenode));
	}
	//Getting the inode position of the file.
	unsigned int location,filepos;
	for(int i=0;i<mountdesc[pos].rootnodevar.size;i++){
		if(strcmp(fnode[i].filename,oldfile)==0){
			location = fnode[i].inodelocation;
			filepos = i;
			break;
		}
	}
	//Getting the meta data of the file.
	inode invar;
	lseek(mountdesc[pos].fd,mountdesc[pos].sbvar.iNodeStart+(location*IN_SIZE),SEEK_SET);
	read(mountdesc[pos].fd,&invar,IN_SIZE);
	//Reading the file for a data node and writing it to the File.
	lseek(mountdesc[pos].fd,mountdesc[pos].sbvar.dataNodeStart+(invar.table[0]*mountdesc[pos].sbvar.blockSize),SEEK_SET);
	for(int i=0;;i++){
		if(invar.table[i]==INVALID)
			break;
		read(mountdesc[pos].fd,buffer,mountdesc[pos].sbvar.blockSize);
		write(fd,buffer,mountdesc[pos].sbvar.blockSize);
	}
	//
	off_t size =(off_t ) invar.size;
	ftruncate(fd,size);
	close(fd);
	copy(destdrive,"$Cache$",newfile);
	delete(srcdrive,oldfile);
	remove("$Cache$");
}

void help(){
	printf("1.Make file system:\n");
	printf("mkfs <Filename> <BlockSize in bytes> <Total Size(eg. 10MB/10KB)>\n");
	printf("2.Mount a file system to a drive:\n");
	printf("mount <Filename> <Drive(eg C:)>\n");
	printf("3.Unmount\n");
	printf("unmount <Drive>\n");
	printf("4.Copy files from osfile system to my file system.\n");
	printf("cp <source filename(eg test.txt)> <Destination Drive with Filename(eg C:/testfile1.txt)>\n");
	printf("5.Display the contents of the file system.\n");
	printf("ls <Drive>\n");
	printf("6.Delete a file from a file system.\n");
	printf("delete <Drive/Filename(C:/testfile1.txt)>\n");
	printf("7.Move file from one Drive to another.\n");
	printf("move <source drive/filename> <destination drive/filename>\n");
	printf("8. To Display list of mounted drives.\n");
	printf("list drives\n");

}

//Display the list of mounted drive.
void mounteddrive(){
	for (unsigned int i = 0; i < MAXDRIVE; ++i){
		if(mountdesc[i].status==true){
			printf("%c:\n",mountdesc[i].drive);
		}
	}
}

// Convert xMB/xKB to bytes.
unsigned long getsize(char ptr[]){
	int len = strlen(ptr);
	unsigned long temp=0;
	for(int i=0;i<len;i++){
		if(ptr[i]>=48&&ptr[i]<=57){
			temp=(10*temp)+(ptr[i]-'0');
		}
		else if(ptr[i]=='M'){
			if(ptr[i+1]=='B'){
				return (temp*1000*1000);
			}
			else{ 
				printf("Wrong Size.\n");
			}
		}
		else if (ptr[i]=='K'){
			if(ptr[i]=='B'){
				return (temp*1000);
			}
			else{
				printf("Wrong Size.\n");
			}
		}
	}
}

//Get the size of the file name.
unsigned long fsize(char* file){

    FILE * f = fopen(file, "r");
    fseek(f, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(f);
    fclose(f);
    return len;
}

//Decode the path(Drive:/filename)
decodestruct decode(char *ptr){

	int len = strlen(ptr);
	decodestruct temp;
	temp.drive = ptr[0];
	for(int i=3;i<len;i++){
		if(ptr[i]!='/')
			temp.filename[i-3]=ptr[i];
	}
	return temp;
}

//Decimal to binary.
void decitobi(int arr[],unsigned char num){

	for(int i=0;i<8;i++){
		arr[i] = num%2;
		num=num/2;
	}
}

//Convert the binary num to a decimal number.
unsigned char bitodeci(int arr[]){

	unsigned char num = 0;
	for(int i=0;i<8;i++){
		num = num*2+arr[i];
	}
	return num;
}

// Get the position of the drive in the descriptor.
unsigned int  getpos(char Drive){
	unsigned int pos;
	for (unsigned int i = 0; i < MAXDRIVE; ++i){
		if(mountdesc[i].status==true){
			if(mountdesc[i].drive==Drive){
				pos = i;
				break;
			}
		}
	}
	return pos;
}