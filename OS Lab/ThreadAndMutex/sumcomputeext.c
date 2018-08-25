/*File:sumcompute.c*/
//Finding the sum of elements of an array parallely by using multiple threads.
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

struct v {
	int *arr;
	int *levelarr;
	int level;
	int i;
	int j;
	};

pthread_mutex_t mutex;

//Sum Function to be executed Parallely.
void *sum(void *data){
	//Creating a temporary struct v pointer.
	struct v *ptr = (struct v *)data;
	int flag = 0;
	while(1){
		pthread_mutex_lock(&mutex);
		if((*((ptr->levelarr)+(ptr->i))==((ptr->level)-1))&&(*((ptr->levelarr)+(ptr->j))==((ptr->level)-1)))
		{
			//Adding the result and storing the result in the i-th index of array.
			*((ptr->arr)+(ptr->i))=*((ptr->arr)+(ptr->i))+*((ptr->arr)+(ptr->j));
			*((ptr->levelarr)+(ptr->i)) = ptr->level;
			*((ptr->levelarr)+(ptr->j)) = ptr->level;
			flag = 1;
		}
		pthread_mutex_unlock(&mutex);
	 	if(flag==1)
			break;
	}
	//Exiting the pthread.
	pthread_exit(0);
}

//Function which computes the sum of elements concurrently using threads.
int sumOfElements(int data[],int size){
	//Size = 1 then return the same element.
	if(size ==1)
		return data[0];

	// Error checking of pthread_create and pthread_join.
	int status;
	//Initialiing a temporary size.
	int tempsize;
	//Pthread Attribute.
	pthread_attr_t attr;
	//Pthread Attribute initialize.
	pthread_attr_init(&attr);
	int i,j,k;
	int level;
	pthread_t master;
	int noofthreads=0;
	tempsize=size;
	while(1){
		noofthreads = noofthreads+tempsize/2;
		if(tempsize%2==0)
			tempsize = tempsize/2;
		else
			tempsize = (tempsize/2)+1;
		if(tempsize==1)
			break;
	}
	int *levelarr = (int *)malloc(sizeof(int)*size);
	for(int i=0;i<size;i++)
		*(levelarr+i) = 0;

	pthread_mutex_init(&mutex, NULL);

	//Creating a array of thread ids.
	pthread_t tid[noofthreads];
	tempsize=size;
	k=2;
	level = 0;
	j=0;
	//Loop to Calculate Sum.
	while(1){
		i=0;
		//Loop to Create Parallel Thread of a single level. 
		level++;
		for(int c=0;c<(tempsize-1);c+=2){
			//Struct v pointer Declaration.
			struct v *ptr;
			ptr = (struct v*)malloc(sizeof(struct v));
			ptr->arr = data;
			ptr->i = i;
			ptr->j = i+(k/2);
			ptr->level = level;
			ptr->levelarr = levelarr;
			//Pthread Create.
			status = pthread_create(&tid[j],&attr,sum,ptr);
			j++;
			i+=k;
			// if (status != 0) {
			// 	fprintf(stderr, "pthread_create() failed: %s.\n", status == EAGAIN?"Insufficient resources to create another thread OR A  system-imposed  limit on the number of threads was encountered.":status == EINVAL?"Invalid settings in attr.":status == EPERM?"No permission to set the scheduling policy and parameters specified in attr.":"Unknown Error");
			// 	exit(1);
			// }
		}
		if(tempsize%2==1)
			*(levelarr+i) = level;
		// increasing the difference in power of 2.
		k=k*2;
		// changing the size of arr.
		if(tempsize%2==0)
			tempsize = tempsize/2;
		else
			tempsize = (tempsize/2)+1;

		//checking for base condition.
		if(tempsize==1)
			break;
	}
	for(i=0;i<noofthreads;i++){
		pthread_join(tid[i],NULL);
	}
	
	//returning the result.
	return data[0];
}

int main(int argc,char const *argv[]){
	int size;//size value.

	printf("Enter the size of array:");
	scanf("%d",&size);

	int *data = (int *)malloc(size*sizeof(int));//array declaration.

	printf("Enter the elements of array:\n");

	for(int i=0;i<size;i++)
		scanf("%d",(data+i));//accepting inputs.

	// for(int i=0;i<size;i++)
	// 	*(data+i)=2*(i+1);

	//returning the result in 'res' variable
	int res = sumOfElements(data,size);
	//printing the result.
	printf("%d\n", res);
	//exiting the thread.
	pthread_exit(0);
	
}