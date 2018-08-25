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
	int i;
	int j;
	};
//Sum Function to be executed Parallely.
void *sum(void *data){
	//Creating a temporary struct v pointer.
	struct v *ptr = (struct v *)data;
	//Adding the result and storing the result in the i-th index of array.
	*((ptr->arr)+(ptr->i))=*((ptr->arr)+(ptr->i))+*((ptr->arr)+(ptr->j));
	//Exiting the pthread.
	pthread_exit(0);
}

//Function which computes the sum of elements concurrently using threads.
int sumOfElements(int data[],int size){
	// Error checking of pthread_create and pthread_join.
	int status;
	//Initialiing a temporary size.
	int tempsize=size;
	//Pthread Attribute.
	pthread_attr_t attr;
	//Pthread Attribute initialize.
	pthread_attr_init(&attr);
	int k=2;
	int i,j;
	int flag;
	//Loop to Calculate Sum.
	while(1){
		//Size = 1 then return the same element.
		if(size ==1)
			return data[0];
		//Flag is maintianed to keep track of the Remainder.
		flag=0;
		//Checking the Remainder.
		if(tempsize%2==1){
			tempsize = tempsize-1;
			flag=1;
		}
		i=0,j=0;
		//Creating a array of thread ids.
		pthread_t tid[tempsize/2];	
		//Loop to Create Parallel Thread. 
		for(int c=0;c<tempsize;c+=2){
			//Struct v pointer Declaration.
			struct v *ptr;
			ptr = (struct v*)malloc(sizeof(struct v));
			ptr->arr = data;
			ptr->i = i;
			ptr->j = i+(k/2);
			//Pthread Create.
			status = pthread_create(&tid[j],&attr,sum,ptr);
			j++;
			if (status != 0) {
				fprintf(stderr, "pthread_create() failed: %s.\n", status == EAGAIN?"Insufficient resources to create another thread OR A  system-imposed  limit on the number of threads was encountered.":status == EINVAL?"Invalid settings in attr.":status == EPERM?"No permission to set the scheduling policy and parameters specified in attr.":"Unknown Error");
				exit(1);
			}
			i+=k;
		}
		j=0;
		for(int c=0;c<tempsize;c+=2){
			//Pthread Join.
			status = pthread_join(tid[j], NULL);
			j++;
			if (status != 0) {
				fprintf(stderr, "pthread_join() failed: %s.\n", status == EDEADLK?"A deadlock was detected (e.g., two threads tried to join with each other); or thread specifies the calling thread.":status == EINVAL?"thread is not a joinable thread OR Another thread is already waiting to join with this thread.":status == ESRCH?"No thread with the ID thread could be found.":"Unknown Error");
				exit(1);
			}
		}
		// increasing the difference in power of 2.
		k=k*2;
		// changing the size of arr.
		tempsize = tempsize/2;
		//Add the remainder to array once again.
		if(flag==1){
			tempsize++;
		}
		//checking for base condition.
		if(tempsize==1)
			break;
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

	//returning the result in 'res' variable
	int res = sumOfElements(data,size);
	//printing the result.
	printf("%d\n", res);
	//exiting the thread.
	pthread_exit(0);
	
}