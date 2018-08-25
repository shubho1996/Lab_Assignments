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

void meanfilter(){
	
	int i=0,j=0;
		for(i=0;i<lnt;i++){
	    	for(j=0;j<brth;j++){
				if(i==0||j==0||i==(lnt-1)||j==(brth-1))
					;
				else
					arr[i][j]=(arr[i-1][j]+arr[i][j-1]+arr[i-1][j-1]+arr[i+1][j]+arr[i][j+1]+arr[i+1][j+1]+arr[i+1][j-1]+arr[i-1][j+1]+arr[i][j])/9;
				}
		}
		
}


void imgwrite(FILE **q){
//	printf("write");
	int i,j;
	fprintf((*q),"%s\n%d %d\n%d\n","P2",lnt,brth,max);
//	fprintf((*q),"%d and %d \n",lnt,lnt);
	for(i=0;i<lnt;i++){
    	for(j=0;j<brth;j++)
      		fprintf((*q),"%d\n",arr[i][j]);
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
	meanfilter();
	    
	FILE *nwpic;
	nwpic = fopen("picresultmean.pnm","w");
	if(nwpic==NULL){
		puts("cannot open file");
		exit(1);
	}
		imgwrite(&nwpic);
	fclose(nwpic);
	return 0;
}
