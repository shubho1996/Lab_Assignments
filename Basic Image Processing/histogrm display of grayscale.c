#include<stdio.h>
#include<stdlib.h>
int arr[3000][3000],histogrmarr[256];
int lnt,brth,max;


void getsize(FILE **q){
	//printf("size");
	fscanf((*q),"%d %d",&lnt,&brth);
}


void imgread(FILE **q){
//	printf("read");
	int i=0,j=0;
			for(i=0;i<lnt;i++)
	     		for(j=0;j<brth;j++)
	     			fscanf((*q),"%d",&arr[i][j]);
				
	    	
	    
}


void crthistogrm(){
	int i=0,j=0;
	for(i=0;i<lnt;i++)
		for(j=0;j<brth;j++)
			histogrmarr[arr[i][j]]++;
}

void dsplyhistogrm(){
		int i=0;
	for(i=0;i<256;i++)
		printf("Frequency of %d pixel:%d\n",i,histogrmarr[i]);
		
	printf("\n\n\n...........End of histogram...........\n\n");
	
}


int main(){
FILE *pic;
pic=fopen("filegray.pnm","r");
	if(pic==NULL){
		puts("cannot open file");
		exit(1);
	}
 // printf("main");
	char skln[60];
	long int i;
	for(i=0;i<2;i++){
		fgets(skln,59,pic);
	}
	getsize(&pic);
		fscanf(pic,"%d",&max);
	imgread(&pic);
	    fclose(pic);
	
	crthistogrm();
	dsplyhistogrm();
	getch();
	return 0;
}
