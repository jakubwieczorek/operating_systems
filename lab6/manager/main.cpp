#include<iostream>
#include"MemoryManager.h"
#include"main.h"
#include "Validator.h"

using namespace std;

enum {EXIT, SHOW_MENU, CREATE_VIRTUAL_DISC, ADD_FILE, CMD_SIZE};

int main()
{
	long cmd;//przechowuje wybory uzytkownika cmd-command
	bool isExit = false;

    MemoryManager memoryManager;

	menu();
	while(1)
	{
        cmd = Validator::check("/1/ wyswietla menu >",2, -1, CMD_SIZE);

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
            case ADD_FILE:
            {
                add_file_menu(memoryManager);
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

void add_file_menu(MemoryManager aManager)
{
    string disc_name = Validator::str("Nazwa dysku (bez formatu)> ", 1000);
    string file_name = Validator::str("Nazwa pliku> ", 1000);

    VirtualDisc* virtualDisc = aManager.openDisc(disc_name);

    aManager.copyToDisc(*virtualDisc, file_name);
}

void virtual_disk_menu(MemoryManager aManager)
{
    string disc_name = Validator::str("Nazwa dysku (bez formatu)> ", 1000);
    long size;

    size = Validator::check("Rozmiar dysku (od 1 do 1024 * 100)> ", 2, 0, VirtualDisc::MAX_DISC_SIZE);

    aManager.createVirtualDisk(disc_name, size);
}

void menu()
{
	cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;
	cout<<"$ Menu:"<<endl;
	cout<<"$ "<<SHOW_MENU<<". Menu /"<<SHOW_MENU<<"/"<<endl;
	cout<<"$ "<<CREATE_VIRTUAL_DISC<<". Stwórz wirtualny dysk /"<<CREATE_VIRTUAL_DISC<<"/"<<endl;
	cout<<"$ "<<ADD_FILE<<". Skopiuj plik na dysk /"<<ADD_FILE<<"/"<<endl;
	cout<<"$ "<<EXIT<<". Wyjdz /"<<EXIT<<"/"<<endl;
	cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;
}
