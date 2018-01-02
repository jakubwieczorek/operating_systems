#include<iostream>
#include<thread>
#include"monitor.h"
#include<cstdlib>
#include<cstdbool>
#include<ctime>
#include<cstdio>

using namespace std;

// do testowania: 2, 1, 2, 1; 3, 1, 2, 1; 3, 2, 2, 1; 4, 5, 2, 2; 4, 3, 2, 2; 5, 4, 2, 3

#define N 5 //
#define K 4 // K stanowisk serwisowych
#define L 2 // liczba tankowan
#define P 3 // priorytet: jezeli inService < P wtedy priorytet maja wjezdajace w.p.p wyjezdajace

Monitor monitor; // jeden monitor bo tylko jeden wspodzielony zasob gdzie moze byc tylko jeden watek na raz - pas
bool isServicePassFree = true;
int inService = 0;
Condition waitingAmountToEnterC;
Condition waitingAmountToLeaveC;

int draw();
void bolid(int aBolidId);
void ride(int aBolidId);
void driveDown(int aBolidId);
void refuelling(int aBolidId);
void freeServicePass();
void driveOut(int aBolidId);

int endAmount = 0;

int draw()
{
	return rand() % 3 + 1; // od 0 do 5
}

void ride(int aBolidId)
{
		
	printf("Jazda (Bolid %d)\n", aBolidId);
	sleep(4);
}

void driveDown(int aBolidId)
{
	cout<<"Zjazd (Bolid "<<aBolidId<<")"<<endl;
	sleep(1);
}

void refuelling(int aBolidId)
{
	cout<<"Tankowanie (Bolid "<<aBolidId<<")"<<endl;
	sleep(2);
}

void freeServicePass()
{	
	isServicePassFree = true;	
}

void driveOut(int aBolidId)
{

	cout<<"Wyjazd (Bolid "<<aBolidId<<")"<<endl;
	sleep(1);
}

void bolid(int aBolidId)
{	
	int l = L;
	while(l--)
	{
		//cout<<"l: "<<l<<endl;

		ride(aBolidId);

		monitor.enter();	
		// jesli jest zajety pass serwisowy lub ilosc bolidow w alei serwisowej jest wieksza, rowna P
		if(!isServicePassFree || inService >= P)
		{
			// wsrodku tej metody jest leave
			monitor.wait(waitingAmountToEnterC);
			
			//cout<<"siema3, "<<aBolidId<<endl;	
			// po koncu oczekiwania zjazd na pas serwisowy
			//monitor.enter();
		}
		isServicePassFree = false;
		driveDown(aBolidId);
		
		// juz zjechal
		freeServicePass();
		// powiadom oczekujacych na wyjazd ze moga wyjechac bo aleja serwisowa jest pusta albo co wazniejsze ma teraz pierwszenstwo 
		monitor.signal(waitingAmountToLeaveC);
		inService++;
		monitor.leave();
	
		refuelling(aBolidId);
		
		// wyjazd:
		monitor.enter();
		// jesli jest zajety pas serwisowy lub pierwszenstwo maja wjezdzajacy i wjezdajacy nie skonczyli wyscigu
		if(!isServicePassFree || (inService < P && endAmount < P))
		{
			//cout<<"aBolidId: "<<aBolidId<<endl;
			//cout<<"isServicePasFree: "<<isServicePassFree<<endl;
			//cout<<"inService: "<<inService<<endl;
			//cout<<"endAmount:"<<endAmount<<endl;
			monitor.wait(waitingAmountToLeaveC);
			// monitor.enter();
		}

		isServicePassFree = false;
		driveOut(aBolidId);
	
		// juz wyjechal
		freeServicePass();
		inService--;
			
		monitor.signal(waitingAmountToEnterC);
		monitor.signal(waitingAmountToLeaveC);
		//cout<<"siema1, "<<aBolidId<<endl;	
		monitor.leave();
		//cout<<"siema2, "<<aBolidId<<endl;	
	}

	//cout<<"siema "<<aBolidId<<endl;
	endAmount++;
}

int main()
{
	srand(time(NULL));
	
	monitor = Monitor();
	
	waitingAmountToEnterC = Condition();
	waitingAmountToLeaveC = Condition();
	
	thread bolids[N];

	for(int i = 0; i < N; i++)
	{
		thread temp(bolid, i);
		bolids[i] = move(temp);
	}
		
	for(int i = 0; i < N; i++)
	{
		bolids[i].join();
	}

	return 0;
}
