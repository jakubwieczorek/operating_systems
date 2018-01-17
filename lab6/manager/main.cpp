#include<iostream>
#include"MemoryManager.h"
#include"main.h"
#include "Validator.h"

using namespace std;

enum {EXIT, SHOW_MENU, CREATE_VIRTUAL_DISC, ADD_FILE, SHOW_CONTENT, SHOW_MAP, DELETE_FILE,
    DELETE_DISC, CMD_SIZE};

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
            case SHOW_CONTENT:
            {
                show_content_menu(memoryManager);
                break;
            }
            case SHOW_MAP:
            {
                show_map_menu(memoryManager);
                break;
            }
            case DELETE_FILE:
            {
                delete_file_menu(memoryManager);
                break;
            }
            case DELETE_DISC:
            {
                delete_disc_menu(memoryManager);
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

    if(!aManager.copyToDisc(*virtualDisc, file_name))
    {
        cout<<"Plik juz istnieje"<<endl;
    }
}

void delete_file_menu(MemoryManager aManager)
{
    string disc_name = Validator::str("Nazwa dysku (bez formatu)> ", 1000);
    string file_name = Validator::str("Nazwa pliku> ", 1000);

    VirtualDisc* virtualDisc = aManager.openDisc(disc_name);

    aManager.deleteFileFromDisc(*virtualDisc, file_name);
}

void delete_disc_menu(MemoryManager aManager)
{
    string disc_name = Validator::str("Nazwa dysku (bez formatu)> ", 1000);

    aManager.deleteDisc(disc_name);
}

void show_content_menu(MemoryManager aManager)
{
    string disc_name = Validator::str("Nazwa dysku (bez formatu)> ", 1000);

    VirtualDisc* virtualDisc = aManager.openDisc(disc_name);

    aManager.showContent(*virtualDisc);
}

void show_map_menu(MemoryManager aManager)
{
    string disc_name = Validator::str("Nazwa dysku (bez formatu)> ", 1000);

    VirtualDisc* virtualDisc = aManager.openDisc(disc_name);

    aManager.showMap(*virtualDisc);
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
	cout<<"$ "<<SHOW_CONTENT<<". Wyswietl zawartosc dysku /"<<SHOW_CONTENT<<"/"<<endl;
	cout<<"$ "<<SHOW_MAP<<". Wyswietl mape pamieci /"<<SHOW_MAP<<"/"<<endl;
	cout<<"$ "<<DELETE_FILE<<". Usun plik /"<<DELETE_FILE<<"/"<<endl;
	cout<<"$ "<<DELETE_DISC<<". Usun dysk /"<<DELETE_DISC<<"/"<<endl;
	cout<<"$ "<<EXIT<<". Wyjdz /"<<EXIT<<"/"<<endl;
	cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$"<<endl;
}
