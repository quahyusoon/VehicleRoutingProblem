#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#define threadsize 4
HANDLE t[threadsize]; 
DWORD WINAPI Distance_Combi(LPVOID lpParam);
FILE *pfile[100];
int s_path[20],combination[1000][50];
int cities=0,amountCombi=0;
int car;
float distanceArr[20][20],min=50000;
struct point {
	int no;
	int x;
	int y;
};
typedef struct point COOR;
COOR city[20];

/*===================writeFiles===================
			Write shortest path into separate file which created in threads
*/
void writeFiles(int s_path[],int size,int car,int cities)
{
	int i;

	for(int fileNum=0,arr_position=0;fileNum<car;fileNum++)
	{
		fprintf(pfile[fileNum],"0\n");
		if(cities>car)
		{
			for (i=arr_position;i<size;i++)
			{
				if(s_path[i]!=0)
					fprintf(pfile[fileNum],"%d\n",s_path[i]);
				else
				{
					arr_position=i+1;
					break;
				}
			}
		}
		else 
		{
			if(cities>0)
			{
				fprintf(pfile[fileNum],"%d\n",fileNum+1);
				cities--;
			}
		}
		fprintf(pfile[fileNum],"0\n");
		fclose(pfile[fileNum]);
	}
}


/*==============generatecombi===========================
			Generate all combinations based on number of cities and vehicles.
			Pre  number of cities and vehicles
			Post generate all combinations of cities  
*/
void generatecombi(int row[],int cityNo,int carNo,int rowPosition, int a){
	if (carNo==1)
	{
		row[rowPosition]=cityNo;
		for(int i=0;i<=rowPosition;i++)
			combination[amountCombi][i]=row[i];
		amountCombi++;
	}
	else if ( rowPosition==0)
	{ 
		row[rowPosition]=a;
		generatecombi(row,cityNo-a,carNo-1,rowPosition+1,a);
	}
	else
	{
		int j=1;
		while(j<=cityNo-carNo+1)
		{
			row[rowPosition]=j;
			generatecombi(row,cityNo-j,carNo-1,rowPosition+1,a);
			j++;
		}
	}
}

/*=====================swap=================
		    Swap the position of element in the array.
			Pre  permu_source
			Post new permutation
*/
void swap ( int permu_src[],int i, int j) {
	int	temp;
	
	temp = permu_src[i];
	permu_src[i] = permu_src[j];
	permu_src[j] = temp;
}

/*================findPathDist====================
			Compute the max distance traveled among vehicles
			Pre  route[]
			Post returns max distance
*/
float findPathDist(int route[],int num){
	int i;
	float totalDistance=0,max=0;
	totalDistance=distanceArr[0][route[0]];
	for (i=0;i<num-1;i++)
	{
		if (route[i+1]==0)
		{
			totalDistance=totalDistance+distanceArr[route[i]][0];
			if(totalDistance>max)
				max=totalDistance;
			totalDistance=distanceArr[0][route[i+2]];
			i++;
		}
		else
			totalDistance=totalDistance+distanceArr[route[i]][route[i+1]];
	}
    totalDistance=totalDistance+distanceArr[route[i]][0];
	if(totalDistance>max)
				max=totalDistance;
	return max;
}

/*==========================perm=======================
		Generate all permutations and splits them by inserting "0" in between 
		Pre  permu_source
		Post generates all possible route
*/
void permute(int permu_src[ ],int i,int j,int num)
{
	int a,temp[30];
	int	w,x,y,z,route[20],position;
	float pathDistance;
	if (j==num+1)
	{
		for(w=0;w<amountCombi;w++){
			position=combination[w][0];
			for (x=0,y=0,z=1;x<num+car-1,y<num;x++,y++)
			{
				route[x]=permu_src[y];
				if (x==position-1)
				{
					route[++x]=0;
					position=position+combination[w][z]+1;
					z++;
				}
			}
			pathDistance=findPathDist(route,num+car-1);
			if(pathDistance<min){
				min=pathDistance;
				for(x=0;x<num+car-1;x++)
					s_path[x]=route[x];
			}
		}
		
	}
	else
	{
		for (a=0;a<j;a++)
			temp[a]=permu_src[a];
		temp[i]=j;
		permute(temp,i+1,j+1,num);
		a=j-1;
		while (a>0 &&j>2)
		{
			swap (temp, a, a-1);
			permute(temp,i+1,j+1,num);
			a--;
		}
	}
}

int main()
{
	
	int no_of_point,x_value,y_value;
	int permu_src[30];
	FILE *inputfile=NULL;
	inputfile=fopen("Map.vrp","r");
	
	if(inputfile==NULL)
	{
		printf("Unable to read the file.\n");
		exit(1);
	}
	
	fscanf(inputfile,"%d",&car);
	while(fscanf(inputfile,"%d%d%d",&no_of_point,&x_value,&y_value)!=EOF)
	
	{
		city[cities].no=no_of_point;
		city[cities].x=x_value;
		city[cities].y=y_value;
		cities++;
	}
	fclose(inputfile);
	if (cities-1>car)
	{
		for ( int thdstart = 0 ; thdstart < threadsize; thdstart++)
		{ 
			t[thdstart] = CreateThread(NULL,0,Distance_Combi,(void*)thdstart,0,NULL);
		}
		WaitForMultipleObjects(threadsize,t,true,INFINITE);
		permute(permu_src,0,1,cities-1);
		
	}
	
	writeFiles(s_path,cities+car-2,car,cities-1);
	
	
	return 0;
}

DWORD WINAPI Distance_Combi(LPVOID lpParam)
{	
		char fileNo[2];
		char filename[20];
		int combi_arr[10],a;
		for( a=(int)lpParam;a<cities;a+=threadsize)
		{
			for (int b=1;b<cities;b++)
			{
				if (b> a)
				{
					distanceArr[a][b]=sqrt(((float)(city[b].x-city[a].x)*(city[b].x-city[a].x))+((city[b].y-city[a].y)*(city[b].y-city[a].y)));
					distanceArr[b][a]=distanceArr[a][b];
				}
			}
			distanceArr[a][a]=0;
		}
		for( a=(int)lpParam;a<cities-car;a+=threadsize)
		{
			generatecombi(combi_arr,cities-1,car,0,a+1);
		}
		for(int a=(int)lpParam;a<car;a+=threadsize)
		{
			itoa(a,fileNo,10);
			strcpy(filename,"Solution");
			strncat(filename,fileNo,10);
			strcat(filename,".vrp");
			pfile[a]=fopen(filename,"w");
		}

		return 0;
}
