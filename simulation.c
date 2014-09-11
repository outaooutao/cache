#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <hiredis/hiredis.h>
#include <string.h>
//-lrt -lhiredis
#define TTL 3600
#define MAX 1000
//START=2014/9/9 22:00:00
int START; 

int line=0;


void handler(union sigval v)
{
	
	printf("handler called\n");
	redisContext *conn = redisConnect("127.0.0.1", 6379);
	int cached;
	
	int* p=(int*)v.sival_ptr;
	int len=*p;
	p++;
	int i;
	int q;
	for(i=0;i<len;i++)
	{
	line++;
	q=*p;
	p++;
	char command[50];
	memset(command,0,50);
	sprintf(command,"exists %d",q);
	redisReply *reply = redisCommand(conn, command);
	cached=(int)reply->integer;
	freeReplyObject(reply);
	if(cached)//hit 1
	{
		printf("%d %d 1\n",line, q);

	}
	else{//miss 0
		memset(command,0,50);
		sprintf(command,"setex %d %d %d",q, TTL, line);
		reply = redisCommand(conn, command);
		printf("%d %d 0\n", line, q);
		freeReplyObject(reply);
	}

	}
	free(v.sival_ptr);
	redisFree(conn);
}


int main()
{
	printf("sb\n");
	START=(int)time(NULL)+30;
	printf("%d\n",START);

	int queryIssue[MAX];
	int queryNum=0;
	int happenTime;


	FILE *fp;
	fp=fopen("mycachelog","r");
	int time,query,qhit;
	int lastTime=-1;
	int i=0;


	while(i<100)
	{
	i++;
	fscanf(fp,"%d",&time);
	fscanf(fp,"%d",&query);
	
	fscanf(fp,"%d",&qhit);
	if(lastTime==time)//many things happened at same time
	{
		queryIssue[queryNum]=query;
		queryNum++;
		
	}
	else{
		lastTime=time;
		if(queryNum>0)
		{
		
		struct sigevent evp;
		memset(&evp,0,sizeof(evp));
		evp.sigev_notify = SIGEV_THREAD;
		evp.sigev_notify_function=handler;
		
		int* data=(int*)malloc((queryNum+1)*sizeof(int));
		*data=queryNum;
		memcpy(data+1, queryIssue, queryNum*sizeof(int));
		evp.sigev_value.sival_ptr=data;
		
		


		timer_t tid;
		struct itimerspec value;
		struct itimerspec ovalue;
		value.it_interval.tv_sec = 0;
		value.it_interval.tv_nsec = 0;
		value.it_value.tv_sec =happenTime+START;
		value.it_value.tv_nsec = 0;
		timer_create(CLOCK_REALTIME, &evp, &tid);
		timer_settime(tid,TIMER_ABSTIME,&value,&ovalue);
		}

		//new time
		queryIssue[0]=query;
		queryNum=1;
		happenTime=time;
	}


	}

	while(1);
	return 0;
}


