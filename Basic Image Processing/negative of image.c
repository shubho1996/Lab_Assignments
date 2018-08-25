#include<stdio.h>
#include<stdlib.h>
long int arr[3000][3000][3];
long int lnt,brth,max;


void getsize(FILE **q){
//	printf("size");
	fscanf((*q),"%d %d",&lnt,&brth);
}


void imgread(FILE **q){
//	printf("read");
	int i=0,j=0,k=0;
			for(i=0;i<lnt;i++)
		{
	     	for(j=0;j<brth;j++)
	     	{	for(k=0;k<3;k++)
					fscanf((*q),"%d",&arr[i][j][k]);
				arr[i][j][0]=255-arr[i][j][0];
				arr[i][j][1]=255-arr[i][j][1];
				arr[i][j][2]=255-arr[i][j][2];
			}
	    }
	//printf("read end");    
}

void imgwrite(FILE **q){
//	printf("write");
	int i,j,k;
	fprintf((*q),"%s\n%d %d\n%d\n","P3",lnt,brth,max);
	//fprintf((*q),"%d and %d \n",lnt,brth);
	for(i=0;i<lnt;i++){
    	for(j=0;j<brth;j++)
    		for(k=0;k<2;k++)
      			fprintf((*q),"%d\n%d\n%d\n",arr[i][j][0],arr[i][j][1],arr[i][j][2]);
     }
     
	//printf("write end");
	
}
int main(){
FILE *pic;		//img file opening
pic=fopen("filecolor.pnm","r");
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
	FILE *nwpic;		//new img file open.
	nwpic = fopen("pic.pnm","w");
	if(nwpic==NULL){
		puts("cannot open file");
		exit(1);
	}
		imgwrite(&nwpic);
	fclose(nwpic);
	printf("job done.");
	return 0;
}
