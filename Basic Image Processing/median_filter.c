#include<stdio.h>
#include<stdlib.h>
long int arr[3000][3000],nwarr[3000][3000];
long int lnt,brth,max;


void getsize(FILE **q){
	//printf("size");
	fscanf((*q),"%d %d",&lnt,&brth);
}


void imgread(FILE **q){
//	printf("read");
	int i=0,j=0,k=0;
	
	for(i=0;i<lnt;i++)
	    for(j=0;j<brth;j++)
			
			fscanf((*q),"%d",&arr[i][j]);
	    
}

int mid(int a[9]){
	int i=0,j=0,temp=0;
	for(i=0;i<9;i++){
		for(j=0;j<9;j++)
			if(a[i]>a[j]){
				temp=a[i];
				a[i]=a[j];
				a[j]=temp;
			}		
	}
	return a[4];
}

void filter(){
	int i=0,j=0,order[9];
		for(i=0;i<lnt;i++){
	    	for(j=0;j<brth;j++){
				if(i==0||j==0||i==(lnt+1)||j==(brth+1))
					nwarr[i][j]=arr[i][j];
				else
				order[0]=arr[i-1][j-1];order[1]=arr[i-1][j];order[2]=arr[i-1][j+1];
				order[3]=arr[i][j-1];order[4]=arr[i][j];order[5]=arr[i][j+1];
				order[6]=arr[i+1][j-1];order[7]=arr[i+1][j];order[8]=arr[i+1][j+1];
				
				nwarr[i-1][j-1]=mid(order);
				}
		}
		
				
}

void imgwrite(FILE **q){
//	printf("write");
	int i,j;
	fprintf((*q),"%s\n%d %d\n%d\n","P2",lnt,brth,max);
//	fprintf((*q),"%d and %d \n",lnt,brth);
	for(i=0;i<lnt;i++){
    	for(j=0;j<brth;j++)
      		fprintf((*q),"%d\n",nwarr[i][j]);
     }
     
//	printf("write end");
	
}
int main(){
FILE *pic;
pic=fopen("filenoise.pnm","r");				
	if(pic==NULL){
		puts("cannot open file");
		exit(1);
	}
  //printf("main");
	char skln[60];
	long int i;
	for(i=0;i<2;i++){
		fgets(skln,59,pic);
	}
	getsize(&pic);
		fscanf(pic,"%d",&max);
	imgread(&pic);
	    fclose(pic);
	filter();
	FILE *nwpic;
	nwpic = fopen("picresultmedian.pnm","w");
	if(nwpic==NULL){
		puts("cannot open file");
		exit(1);
	}
		imgwrite(&nwpic);
	fclose(nwpic);
	return 0;
}
