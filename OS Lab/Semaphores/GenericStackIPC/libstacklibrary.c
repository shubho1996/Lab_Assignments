#include "stackheader.h"

void initialize(){
	ptr = (struct stackdesc *)shmat(stkdescshmid,NULL,0);
	for(int i=0;i<Max_Stack;i++){
		(*(ptr+i)).free=true;
	}
	shmdt(ptr);
}

void semopset(int stackid){
    Pop.sem_num = stackid;
    Pop.sem_op = -1;
    Pop.sem_flg = SEM_UNDO;

    Vop.sem_num = stackid;
    Vop.sem_op = 1;
    Vop.sem_flg = SEM_UNDO;
}


void structcreation(){

	size_t size = Max_Stack*(sizeof(struct stackdesc));
	// key_t ftok(const char *pathname, int proj_id);
	key_t key = ftok("/home/shubho/Documents/New/KEY/abc.txt",5);

	semid = semget(key,Max_Stack,IPC_CREAT|IPC_EXCL|0777);

	if(semid==-1)
		semid = semget(key,Max_Stack,IPC_CREAT|0777);
	else{
		//int semctl(int semid, int semnum, int cmd, ...);
		setvalArg.val = 1; 
		semctl(semid,0, SETVAL, setvalArg);
	}

	stkdescshmid = shmget(key,size,IPC_CREAT|IPC_EXCL|0777);
	if(stkdescshmid==-1){
		stkdescshmid = shmget(key,size,IPC_CREAT|0777);
	}
	else
		initialize();
	ptr = (struct stackdesc *)shmat(stkdescshmid,NULL,0);	
}

void* stackcreate(int sid,key_t key,size_t ds,int ss){

	int shmid = shmget(key,(ds*ss),IPC_CREAT|0777);
	(*(ptr+sid-1)).shmid = shmid;
	return shmat(shmid,NULL,0);
}

int shstackget(key_t key,size_t ds,int ss){
	
	structcreation();

	for(int i=0;i<Max_Stack;i++){
		if((*(ptr+i)).free==false){
			if((*(ptr+i)).stackKey==key){
				//(*(ptr+i)).stack_ptr = stackcreate(i+1,key,ds,ss);
				//(*(ptr+i)).stack_top =(char *)((*(ptr+i)).stack_ptr+(*(ptr+i).num)*(*(ptr+i)).data_size);
				return i+1;
				universe = 1;
			}		
		}
	}

	int flag = 0;

	for(int i=0;i<Max_Stack;i++){
		if((*(ptr+i)).free == true){
			semopset(i);
			P(semid);
			(*(ptr+i)).stackKey = key;
			(*(ptr+i)).data_size = ds;
			(*(ptr+i)).stack_size = ss;
			(*(ptr+i)).stack_ptr = stackcreate(i+1,key,ds,ss);
			(*(ptr+i)).stack_top =(char *)(*(ptr+i)).stack_ptr;
			(*(ptr+i)).free = false;
			(*(ptr+i)).num=0;
			V(semid);
			flag = 1;
			universe = 1; 
			return i+1;
		}
	}
	if(flag==0){
		return -1;
	}	
}

int shstackpush(int sid,void *srcptr){
	if(universe!=1)
		return -2;
	if((*(ptr+sid-1)).num==(*(ptr+sid-1)).stack_size)
		return -1;
	semopset(sid-1);
	P(semid);
	char *temp;
	temp = (*(ptr+sid-1)).stack_top;
	memcpy(temp,srcptr,(*(ptr+sid-1)).data_size);
	temp+=(*(ptr+sid-1)).data_size;
	(*(ptr+sid-1)).stack_top = temp;
	(*(ptr+sid-1)).num++;
	V(semid);
	return 0;
}

int shstackpop(int sid,void *destptr){
	if(universe!=1)
		return -2;
	if((*(ptr+sid-1)).num==0)
		return -1;
	char *temp;
	semopset(sid-1);
	P(semid);
	temp = (*(ptr+sid-1)).stack_top;
	temp-=(*(ptr+sid-1)).data_size;
	memcpy(destptr,temp,(*(ptr+sid-1)).data_size);
	(*(ptr+sid-1)).stack_top = temp;
	(*(ptr+sid-1)).num--;
	V(semid);
	return 0;
}

int shstackrm(int sid,int flag){
	if(universe!=1)
		return -2;

	if(flag == SHSTACK_DETACH){
		shmdt((*(ptr+sid-1)).stack_ptr);
	}
	else if(flag == SHSTACK_RMID){
		shmdt((*(ptr+sid-1)).stack_ptr);
		shmctl((*(ptr+sid-1)).shmid,IPC_RMID,NULL);
		(*(ptr+sid-1)).free = true;
		(*(ptr+sid-1)).num = 0;
	}
	else if(flag == SHSTACK_CMPL){
		int f=0;
		shmdt((*(ptr+sid-1)).stack_ptr);
		shmctl((*(ptr+sid-1)).shmid,IPC_RMID,NULL);
		(*(ptr+sid-1)).free = true;
		for(int i=0;i<Max_Stack;i++){
			if((*(ptr+i)).free==false){
				f = 1;
				break;
			}
		}
		if(f ==0){
			shmdt(ptr);
			shmctl(stkdescshmid,IPC_RMID,NULL);
			semctl(semid,Max_Stack,IPC_RMID);
		}
		else 
			return -1;
	}
}
