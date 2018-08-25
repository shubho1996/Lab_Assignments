#include<stdio.h>
#include<stdlib.h>
int arr[3000][3000],nwarr[3000][3000];
long int histogrmarrfloat[256][2], lnt,brth,max;


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
	//create histogram 
	int i=0,j=0;
	for(i=0;i<lnt;i++)
		for(j=0;j<brth;j++)
			histogrmarr[arr[i][j]][0]++;
}

void histogrmeqlization(){
	//histo gram equilization technique
	float  total=0.0;
	int i,j,k;
	
	for(i=0;i<255;i++){
		for(j=0;j<=i;j++)
			histogrmarr[i][1]+=histogrmarr[j][0];			//Calculating Cumulative Distributive Function
		
		histogrmarr[i][2]=(int)(((double)histogrmarr[i][1]*255/(lnt*brth))+0.5);		//calculating new intensity value.
	}
	
	for(i=0;i<lnt;i++)
		for(j=0;j<brth;j++)
			nwarr[i][j]=histogrmarr[arr[i][j]][2];
	
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
pic=fopen("filegray.pnm","r");
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
	FILE *nwpic;
	nwpic = fopen("pichistgrmequi.pnm","w");
	if(nwpic==NULL){
		puts("cannot open file");
		exit(1);
	}
		imgwrite(&nwpic);
	fclose(nwpic);
	return 0;
}
