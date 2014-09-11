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
FILE *fp;
int line=0;
int qtime,query;


void handler(union sigval v)
{	
	
	int now=(int)time(NULL);
	redisContext *conn = redisConnect("127.0.0.1", 6379);
	while(qtime + START <= now )
	{
		
		int cached;
	
		line++;
		
		
		char command[50];
		memset(command,0,50);
		sprintf(command,"exists %d",query);
		redisReply *reply = redisCommand(conn, command);
		cached=(int)reply->integer;
		freeReplyObject(reply);
		if(cached)//hit 1
		{
			printf("%d %d %d 1\n",line, qtime, query);

		}
		else{//miss 0
			memset(command,0,50);
			sprintf(command,"setex %d %d %d",query, TTL, line);
			reply = redisCommand(conn, command);
			printf("%d %d %d 0\n", line, qtime, query);
			freeReplyObject(reply);
		}	
	
		fscanf(fp,"%d",&qtime);
		fscanf(fp,"%d",&query);
	}
	
	redisFree(conn);
	fflush(stdout);
}

int main()
{
	fp=fopen("logs","r");
	fscanf(fp,"%d",&qtime);
	fscanf(fp,"%d",&query);

	START=(int)time(NULL)+10;
	struct sigevent evp;
	memset(&evp,0,sizeof(evp));
	evp.sigev_notify = SIGEV_THREAD;
	evp.sigev_notify_function=handler;
	
	timer_t tid;
	struct itimerspec value;
	struct itimerspec ovalue;
	value.it_interval.tv_sec = 1;
	value.it_interval.tv_nsec = 0;
	value.it_value.tv_sec =1;
	value.it_value.tv_nsec = 0;
	timer_create(CLOCK_REALTIME, &evp, &tid);
	timer_settime(tid,0,&value,&ovalue);
	
	while(1);
	fclose(fp);
}



