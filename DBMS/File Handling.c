#include <stdio.h>
#include <stdlib.h>

struct student {
	char name[100];
	int roll;
	int marks;
}s[10];

void sort(struct student s[]){
	int flag= s[0].marks;
	int pos=0,i,j;
	struct student temp;
	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			if(flag<s[i].marks){
				temp =s[pos];
				s[pos]=s[i];
				s[i]=temp;
				pos=i;
			}
		}pos=0;
	}
}

int max(struct student s[]){
	int max = s[0].marks;
	int pos = 0, i;
	for(i=0;i<10;i++){
		if(max<s[i].marks){
			max=s[i].marks;
			pos=i;
		}
	}
	printf("%d", s[pos].marks);
	return pos;
}

int min(struct student s[]){
	int min = s[0].marks;
	int pos = 0, i;
	for(i=0;i<10;i++){
		if(min>s[i].marks){
			min=s[i].marks;
			pos=i;
		}
	}
	return pos;
}

void display(struct student s[]){
	int i;
	for(i=0;i<10;i++){
		printf("Student %d :\n",i);
		printf("\t Name : %s\n",s[i].name);
		printf("\t Roll : %d\n",s[i].roll);
		printf("\t Marks : %d\n",s[i].marks);
	}
}
void sortroll(struct student s[]){
	int flag= s[0].roll;
	int pos=0,i,j;
	struct student temp;
	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			if(flag<s[i].roll){
				temp =s[pos];
				s[pos]=s[i];
				s[i]=temp;
				pos = i;
			}
		}pos=0;
	}
}

int main(){
	FILE *ptr;
	ptr = fopen("File.txt","r");
	
	//student s[10];
	int i=0;
	while(i<10){
	
		fscanf(ptr,"%s %d %d",&s[i].name,&s[i].roll,&s[i].marks);
		i++;
		
	}
	int temp;
	int m;
	while(1){
	printf("Enter Your Choice:\n1.Maximum marks\n2.Minimum Marks\n3.Sort Acc. to Marks\n4.Sort Acc. to Roll\n5.Exit\n");
	scanf("%d",&m);
		switch(m){
			case 1: temp = max(s);
					printf("Student With Maximum Marks: %d\n", s[temp].roll);
					printf("\t Name : %s\n",s[temp].name);
					printf("\t Roll : %d\n",s[temp].roll);
					printf("\t Marks : %d\n",s[temp].marks);
					break;
			case 2: temp = min(s);
					printf("Student With Minimum Marks:\n");
					printf("\t Name : %s\n",s[temp].name);
					printf("\t Roll : %d\n",s[temp].roll);
					printf("\t Marks : %d\n",s[temp].marks);
					break;
			case 3: sort(s);
					printf("Sorted Result:\n");
					display(s);
					break;
			case 4: sortroll(s);
					printf("Sorted Result:\n");
					display(s);
					break;
			case 5: return 0;
			default:printf("\nInvalid!!!!\n");
		}
	}
	fclose(ptr);
		
}


