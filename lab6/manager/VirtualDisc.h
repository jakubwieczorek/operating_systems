//
// Created by jakub on 13.01.18.
//

#ifndef MANAGER_VIRTUALDISC_H
#define MANAGER_VIRTUALDISC_H

#include<iostream>
#include<fstream>
#include <list>
#include "VirtualFile.h"
#include "VirtualHole.h"

using namespace std;

class VirtualDisc {
private:
    string discName;
    long freeSpace;
    long discSize;
public:
    list<VirtualFile> files;
    list<VirtualHole> holes;
    static const string DISC_EXTENSION;
    static const string SUFFIX;
    static const long MAX_DISC_SIZE;

    VirtualDisc(const string &discName);

    VirtualDisc();

    const string &getDiscName() const;

    void setDiscName(const string &discName);

    long getFreeSpace() const;

    void setFreeSpace(long freeSpace);

    static const string &getDISC_EXTENSION();

    static const long getMAX_DISC_SIZE();

    long getDiscSize() const;

    void setDiscSize(long discSize);

    const list <VirtualFile> &getFiles() const;

    void setFiles(const list <VirtualFile> &files);

    const list <VirtualHole> &getHoles() const;

    void setHoles(const list <VirtualHole> &holes);

    static const string &getSUFFIX();
};


#endif //MANAGER_VIRTUALDISC_H
