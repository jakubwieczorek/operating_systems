#include "monitor.h"
#include <stdio.h>


#define CONSUMER_A 0
#define CONSUMER_B 1
#define SIZE 9

class PCMonitor : Monitor
{
  private:
	char buffer[SIZE];		//buffer
	int count;				// count - elements in buffer
	int in;					// in - where to write
	int out; 				// out - from where to read
	// Prod - buffer is full
	// ConA - A has already consumed
	// ConB - B has already consumed
	Condition Prod;
	Condition ConA;
	Condition ConB;
	//alrA(B) - A(B) has already read
	int alrA;
	int alrB;
	int waitA;
	int waitB;
  public: 
	PCMonitor();
	void put(char value);
	char get(int c_id);
};

PCMonitor::PCMonitor()
{
  in=out=count=0;    
  alrA=alrB=0;
  waitA=waitB=0;
}

void PCMonitor::put(char value)
{
  enter();
  
  if(count==SIZE)
	wait(Prod);
  buffer[in]=value;
  in=(in+1)%SIZE;
  ++count;
  printf("dodaje element: %c\n",value);
  if(count>3)
  {
	if(alrA)
		signal(ConB);
	else if(alrB)
		signal(ConA);
	else
	if(waitA)
	  signal(ConA);
	else if(waitB)
	  signal(ConB);
  }
  else
    signal(Prod);
  leave();
}

char PCMonitor::get(int c_id)
{
  enter();
  int flag=0; 
  char value;
  // if A has already read and he wants to read - A waits
  if((alrA==1 || count<=3) && c_id==CONSUMER_A)
  {
	waitA++;
	wait(ConA);  
  }
  // if B has already read and he wants to read - B waits
  if((alrB==1 || count<=3) && c_id==CONSUMER_B)
  {
	waitB++;
	wait(ConB);
  }
  
  value=buffer[out];
  if(c_id==CONSUMER_A)
  {
    printf("reading A: %c\n",value);
	alrA=1;
	if(waitA>0)
	  waitA--;
  }
  else
  {
    printf("reading B: %c\n",value);
	alrB=1;
	if(waitB>0)
	  waitB--;
  }
  if(alrB && alrA)
  {
	alrA=alrB=0;
    out=(out+1)%SIZE;
	count--;
	flag=1;
	printf("usunalem element z buffora: %c\n",value);
  }
  if(flag) 
	signal(Prod);
  if(c_id==CONSUMER_A)
  {	
    signal(ConB);
  }
  else
  {
	signal(ConA);
  }
  leave();
  return value;
}

PCMonitor mon;

void * producer(void * args)
{
  while(true)
  {
	char x=*((char *)args);
//	printf("A wyprodukowalem %c\n",x);
	mon.put(x);
	sleep(rand()%5);
  }
}

void * consumer(void * args)
{
  char n=*((char *)args);
  while(true)
  {
    char x;
    if(n=='A')
      x=mon.get(0);
    else
      x=mon.get(1);
//	printf("B_skonsumowalem %c\n",x);
	sleep(rand()%3);
  }  
}

int main()
{
  pthread_t prodA, consA, prodB,consB;
  char a='A';
  char b='B';  
  pthread_create(&prodA,NULL,producer, &a);
  pthread_create(&consA,NULL,consumer, &a);
  pthread_create(&prodB,NULL,producer, &b);
  pthread_create(&consB,NULL,consumer, &b);  
  
  pthread_join(prodA,NULL);
  pthread_join(consA,NULL);
  pthread_join(prodB,NULL);
  pthread_join(consB,NULL);
  
  return 0;
}
