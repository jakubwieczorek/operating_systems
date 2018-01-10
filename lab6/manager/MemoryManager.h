//
// Created by jakub on 09.01.18.
//

#ifndef MANAGER_MEMORYMANAGER_H
#define MANAGER_MEMORYMANAGER_H

#include<iostream>
#include <fstream>

using namespace std;

class MemoryManager
{
private:
    string memoryFormat = ".vd";
    int blockSize = 100;
    long maxDiskSize = 1024 * 100;

public:

    bool createVirtualDisk(string aFileName, long aDiskSpace)
    {
        /*mamy stringa i chcemy z niego zrobic char* to .c_str() trunc zeruje zawartosc podczas zapisu out umozliwia zapis
        in odczyt ate/app zapis na koncu*/
        ofstream file(aFileName.c_str() + memoryFormat, ios::trunc | ios::out);

        for(long i = 0; i < aDiskSpace; i++)
        {
            if(i % blockSize == 0)
            {
                file<<endl;
            }

            file<<"0";
        }

        /*trzeba sprawdzic czy pliki zostaly otworzone obie funkcje zwracaja true gdy uzyskalem dostep do pliku.
        zalezy tez od parametrow otwarcia gdy mam dostep tylko do odczytu a zechce pisac to zostanie ustawiona odpowiednia flaga*/
        if(!file.is_open() || !file.good())
        {
            return false;
        }

        file.close();

        return true;
    }

    long getMaxDiskSize() const
    {
        return maxDiskSize;
    }
};


#endif //MANAGER_MEMORYMANAGER_H
