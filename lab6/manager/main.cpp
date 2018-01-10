#include<iostream>
#include<cstdarg>
#include"MemoryManager.h"
#include"main.h"
#include<sstream>

using namespace std;

enum {EXIT, SHOW_MENU, CREATE_VIRTUAL_DISC, CMD_SIZE};

int main()
{
	long cmd;//przechowuje wybory uzytkownika cmd-command
	bool isExit = false;

    MemoryManager memoryManager;

	menu();
	while(1)
	{
        cmd = check("/1/ wyswietla menu >",2, -1, CMD_SIZE);

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
            case CREATE_VIRTUAL_DISC:
            {
                virtual_disk_menu(memoryManager);
                break;
            }
            default:
            {
                menu();
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

void virtual_disk_menu(MemoryManager aManager)
{
    string file_name;
    long size;

    str(file_name, "Nazwa dysku (bez formatu)> ", 20);
    string text;

   // stringstream strstream;
   // strstream << "Rozmiar dysku (od 1 do " << aManager.getMaxDiskSize() << ")>";
   // strstream >> text;

    size = check("Rozmiar dysku (od 1 do 1024 * 100)> ", 2, 0, aManager.getMaxDiskSize());

    aManager.createVirtualDisk(file_name, size);
}

void str(string &name, const char *msg, int a_size)
{
    while(true)
    {
        cout<<msg;
        cin>>name;
        if(name.size() <= a_size && name.size() >= 1)
        {
            break;
        }
        continue;
    }
}

long check(const char *str, int n, ...)//n ilosc dodatkowych argumentow, pierwszy dod min drugi max.
{
    long to_check;

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

    return to_check;
}

void menu()
{
	cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;
	cout<<"$ Menu:"<<endl;
	cout<<"$ "<<SHOW_MENU<<". Menu /"<<SHOW_MENU<<"/"<<endl;
	cout<<"$ "<<CREATE_VIRTUAL_DISC<<". Stwórz wirtualny dysk /"<<CREATE_VIRTUAL_DISC<<"/"<<endl;
	cout<<"$ "<<EXIT<<". Wyjdz /"<<EXIT<<"/"<<endl;
	cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;
}
