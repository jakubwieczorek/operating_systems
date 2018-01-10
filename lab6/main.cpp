#include<iostream>
#include<cstdarg>
#include<stdio.h>
#include"main.h"

using namespace std;

enum {EXIT, SHOW_MENU, CMD_SIZE};

int main()
{
	short cmd;//przechowuje wybory uzytkownika cmd-command
	bool isExit = false;	

	menu();		
	while(1)
	{
        	check(cmd, "Co teraz ? /1/ wyswietla menu: ",2, -1, CMD_SIZE);
	
		switch(cmd)
		{
			case SHOW_MENU:
			{
				menu();
				break;
			}
			case EXIT:
			{
				isExit = true;
				break;
			}
		}	

		if(isExit)
		{
			break;
		}
	}
	
	return 0;
}

void create_virtual_disk(int a_size, int a_blocks)
{
	
}

void check(short &to_check, const char *str, int n, ...)//n ilosc dodatkowych argumentow, pierwszy dod min drugi max.
{
	va_list ap;//tworzy zmienna ap typu va_list
	va_start(ap, n);//udostępnia parametry znajdujące się po parametrze n

	int min, max; //zmienne potrzebne do tego, że va_arg po każdym wywołaniu przesówa wskaźnik o jeden więc w przypadku pomyłki jest
	//problem

	min=va_arg(ap, int);
	if(--n) max=va_arg(ap, int);// gdy chcemy ograniczyc z gory

	while(true)
	{
		cin.clear();

        cout<<str;//wyswietla wiadomosc z parametru
		cin>>to_check;//pobiera wartosc do sprawdzenia

		if(cin.fail())//jesli blad wprowadzenia
		{
            while(fgetc(stdin)!='\n');//czyszczenie bufora
            cin.clear();
            continue;
		}

		if(to_check<=min)
		{continue;}//gdy jest mniejsze od minimum

		if(n)
		if(to_check>=max)
		{continue;}//gdy jest wieksze od max

		break;
	}
	va_end(ap);
	while(cin.get()!='\n');//czyszczenie bufora
	cin.clear();
}

void menu()
{
	cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;
	cout<<"$ Menu:"<<endl;
	cout<<"$ "<<SHOW_MENU<<". Menu /"<<SHOW_MENU<<"/"<<endl;
	cout<<"$ "<<EXIT<<". Wyjdz /"<<EXIT<<"/"<<endl;
	cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;
}
