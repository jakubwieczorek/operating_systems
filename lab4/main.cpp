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
#define K 1 // K stanowisk serwisowych
#define L 2 // liczba tankowan
#define P 1 // priorytet: jezeli inService < P wtedy priorytet maja wjezdajace w.p.p wyjezdajace

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
		} else if(waitingAmountToEnterC.signal()) // gdy nikt nie chce wyjechac
		{
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
			
		} else // jesli nie to ustaw sie w kolejce do wjazdu
		{	
			cout<<endl<<endl<<"1czekam:         (Bolid "<<aBolidId<<")"<<endl<<endl;
			monitor.wait(waitingAmountToEnterC);	
			cout<<endl<<endl<<"1juz nie czekam: (Bolid "<<aBolidId<<")"<<endl<<endl;
		}

		driveDown(aBolidId); // zjezdza

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
		} else
		{
			cout<<endl<<endl<<"1czekam:         (Bolid "<<aBolidId<<")"<<endl<<endl;
			monitor.wait(waitingAmountToLeaveC);
			cout<<endl<<endl<<"1juz nie czekam: (Bolid "<<aBolidId<<")"<<endl<<endl;
		}

		driveOut(aBolidId); // wyjezdza
	
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
