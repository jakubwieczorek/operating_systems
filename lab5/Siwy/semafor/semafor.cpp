#include <stdio.h>
#include "monitor.h"


#define CONSUMER_A 0
#define CONSUMER_B 1
#define SIZE 9

char buffer[SIZE];
int count;
int in;
int out;
int alrA;
int alrB;
int waitA;
int waitB;

Semaphore empty= Semaphore(0);
Semaphore full= Semaphore(SIZE);
Semaphore turnA= Semaphore(1);
Semaphore turnB= Semaphore(1);
Semaphore mutex=Semaphore(1);

void * producer(void *args)
{
    while(true)
    {
	char value=*((char *)args);
	
	full.p();
	mutex.p();
	printf("P%c w SK -------------- BUFFER: %d\n",value,count);
	buffer[in]=value;
	in=(in+1)%SIZE;
	count++;
	printf("%c +\n", value);
	printf("opuszcza sekcje krytyczna\n");
	if(count>3)
	    empty.v();
	mutex.v();
//	if(count<9)
//	    full.v();
	sleep(rand()%5);
    }
}


void * consumerA(void * args)
{
    while(true)
    {
	int flag=0;
	char value;
    
        turnB.p();
	turnB.v();
    
        empty.p();
	empty.v();
    
        mutex.p();
        printf("KA w SK -------------- BUFFER: %d\n",count);
        value=buffer[out];    
        printf("%c ..\n", value);
	if(alrB)
	{
    	    alrA=alrB=0;
	    flag=1;
	    out=(out+1)%SIZE;
	    printf("%c -\n",value);
	    count--;
	}
	else
	    alrA=1;
	printf("opuszcza sekcje krytyczna\n");
//	mutex.v();
	if(flag)
	    full.v();
//	if(alrA)
	turnB.p();
	turnA.v();
	mutex.v();
	sleep(rand()%5);
    }
}

void * consumerB(void * args)
{

    while(true)
    {
	int flag=0;
	char value;

	turnA.p();
	turnA.v();
    
        empty.p();
        empty.v();
    
        mutex.p();
        printf("KB w SK -------------- BUFFER: %d\n",count);
        value=buffer[out];    
        printf("%c ..\n", value);
        if(alrA)
        {
	    alrA=alrB=0;
	    flag=1;
	    printf("%c -\n",value);
	    out=(out+1)%SIZE;
	    count--;
	}
	else
    	    alrB=1;
	printf("opuszcza sekcje krytyczna\n");
	if(flag)
	    full.v();
//	if(alrB)
	turnA.p();
	turnB.v();
	mutex.v();
	sleep(rand()%5);
    }    
}

int main()
{
    in=out=count=0;
    alrA=alrB=0;
    waitA=waitB=0;
    srand(time(0));    
    pthread_t prodA, consA, prodB, consB;
    char a='A';
    char b='B';
    
    pthread_create(&prodA, NULL, producer, &a);
    pthread_create(&prodB, NULL, producer, &b);
    pthread_create(&consA, NULL, consumerA, &a);
    pthread_create(&consB, NULL, consumerB, &a);

    pthread_join(consB,NULL);
    pthread_join(consA,NULL);
    pthread_join(prodA,NULL);
    pthread_join(prodB,NULL);    
}
