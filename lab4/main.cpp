#include<iostream>
#include<thread>
#include"monitor.h"
#include<cstdlib>
#include<cstdbool>
#include<ctime>
#include<cstdio>

using namespace std;

// do testowania: 2, 1, 2, 1; 3, 1, 2, 1; 3, 2, 2, 1; 4, 5, 2, 2; 4, 3, 2, 2; 5, 4, 2, 3

#define N 3 //
#define K 2 // K stanowisk serwisowych
#define L 2 // liczba tankowan
#define P 1 // priorytet: jezeli inService < P wtedy priorytet maja wjezdajace w.p.p wyjezdajace ale gdy sa np. 2 miejsca a priorytet jest 1 i nikt nie chce wyjechac to moga wjechac mimo ze przekroczy P

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
	if(inService < P) // pierwszenstwo wjezdzajacy
	{	
		if(waitingAmountToEnterC.signal()) // jesli ktos chce wjechac
		{
			monitor.enter();
		} else if(waitingAmountToLeaveC.signal()) // gdy nikt nie chce wjechac
		{
			monitor.enter();
		}
	} else 
	{
		if(waitingAmountToLeaveC.signal()) // jesli ktos chce wyjechac
		{
			monitor.enter();
		} else if(inService < K && waitingAmountToEnterC.signal()) // gdy nikt nie chce wyjechac i jest miejsce
		{	// cout<<endl<<endl<<"Bo nikt nie chce wyjechac i jest miejsce"<<endl<<endl;
			monitor.enter();
		}
	}
	
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
		ride(aBolidId);
		
		// wjazd
		monitor.enter();	
		// jesli jest wolny pas serwisowy i jest miejsce w alei to zajmij pas:
		if(isServicePassFree && inService < K)
		{
			isServicePassFree = false;
			monitor.leave(); // bo wykonalismy juz operacje na zmiennej wspoldzielonej	
		} else // jesli nie to ustaw sie w kolejce do wjazdu
		{	
		 	// cout<<endl<<endl<<"1czekam:         (Bolid "<<aBolidId<<")"<<endl<<endl;
			monitor.wait(waitingAmountToEnterC);	
			// cout<<endl<<endl<<"1juz nie czekam: (Bolid "<<aBolidId<<")"<<endl<<endl;
			monitor.leave();
		}

		driveDown(aBolidId); // zjezdza
		monitor.enter();
		inService++; // zjechal i zajal stanowisko			
		freeServicePass(); // zwolnij pas i powiadom albo wjezdzajacych albo wyjezdzajacych w zaleznosci od priorytetu
		
		monitor.leave(); 
	
		refuelling(aBolidId);
		
		// wyjazd:
		monitor.enter();
		// jesli jest wolny pas serwisowy to wyjedz
		if(isServicePassFree)
		{
			isServicePassFree = false;
			monitor.leave();
		} else
		{
			// cout<<endl<<endl<<"1czekam:         (Bolid "<<aBolidId<<")"<<endl<<endl;
			monitor.wait(waitingAmountToLeaveC);
			// cout<<endl<<endl<<"1juz nie czekam: (Bolid "<<aBolidId<<")"<<endl<<endl;
			monitor.leave();
		}

		driveOut(aBolidId); // wyjezdza
	
		monitor.enter();
		inService--;
		freeServicePass();
			
		monitor.leave();
	}

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
