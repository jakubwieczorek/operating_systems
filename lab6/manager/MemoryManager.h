//
// Created by jakub on 09.01.18.
//

#ifndef MANAGER_MEMORYMANAGER_H
#define MANAGER_MEMORYMANAGER_H

#include<iostream>
#include <fstream>
#include "VirtualDisc.h"

using namespace std;

class MemoryManager
{
private:
    long countLines(string aString);
public:

    bool createVirtualDisk(string aDiscName, long aDiskSpace);
    bool copyToDisc(VirtualDisc& aDisc, string aPath);
    bool copyFromDisc(VirtualDisc aDisc, string aPath);
    bool save(VirtualDisc& aDisc);
    VirtualDisc* openDisc(string aDiscName);
    bool showContent(VirtualDisc aDisc);

    bool showMap(VirtualDisc aDisc);

    bool deleteFileFromDisc(VirtualDisc aDisc, string aFileName);

    bool deleteDisc(string aDiscName);
};


#endif //MANAGER_MEMORYMANAGER_H
