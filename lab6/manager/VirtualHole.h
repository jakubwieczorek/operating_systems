//
// Created by jakub on 15.01.18.
//

#ifndef MANAGER_VIRTUALHOLE_H
#define MANAGER_VIRTUALHOLE_H
#include <iostream>

using namespace std;

class VirtualHole {
private:
    long address;
    long size;

public:
    static const string DESCRIPTOR;

    long getAddress() const;

    void setAddress(long address);

    long getSize() const;

    void setSize(long size);
};


#endif //MANAGER_VIRTUALHOLE_H
