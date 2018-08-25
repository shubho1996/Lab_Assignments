#include<stdio.h>
#include<stdlib.h>
long int arr[3000][3000][3];
int main(){
FILE *pic;
pic=fopen("file.pnm","r");
	if(pic==NULL){
		puts("cannot open file");
		exit(1);
	}
	char skln[100];
	long int lnt,brth,i,j,k;
	for(i=0;i<2;i++){
		fgets(skln,99,pic);
	}
	int max;
	fscanf(pic,"%d %d",&lnt,&brth);
	fscanf(pic,"%d",&max);
	
		for(i=0;i<lnt;i++)
		{
	     	for(j=0;j<brth;j++)
	     	{
		    	for(k=0;k<3;k++)
					fscanf(pic,"%d",&arr[i][j][k]);
				
				arr[i][j][0]=(arr[i][j][0]+arr[i][j][1]+arr[i][j][2])/3;
			}
	    }
	    fclose(pic);
	FILE *q;
	q = fopen("pic2.pnm","w");
	if(q==NULL){
		puts("cannot open file");
		exit(1);
	}
	fprintf(q,"%s\n%d %d\n%d\n","P2",lnt,brth,max);
	printf("%d and %d \n",lnt,brth);
	printf("%s",skln);
	for(i=0;i<lnt;i++)
	{
     for(j=0;j<brth;j++)
      fprintf(q,"%d\n",arr[i][j][0]);
     }
	fclose(q);
	return 0;
}
