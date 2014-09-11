#include <stdio.h>
#define TTL 3600
#define Max 100
int cache[Max];


int main()
{
	/*
	int i;
	for(i=0;i<Max;i++)
	printf("%d\n",cache[0]);
	*/
	FILE *fp;
	fp=fopen("querysortlogs","r");
	int time,query;
	int i=0;
	int j;
	while(i<10000)
	{
	i++;
	fscanf(fp,"%d",&time);
	fscanf(fp,"%d",&query);
	
	if(cache[query])
	{
		if(cache[query]>time)//hit 1
		printf("%d %d 1\n",time,query);
		else
		{
		printf("%d %d 0\n",time,query);//miss
		cache[query]=TTL+time;
		}
	}
	else{//miss 0
		printf("%d %d 0\n",time,query);
		cache[query]=TTL+time;
	}
	
	
	
	}
	
	return 0;
}
