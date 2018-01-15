//
// Created by jakub on 15.01.18.
//

#include "VirtualHole.h"

const string VirtualHole::DESCRIPTOR = "HOLE";


long VirtualHole::getAddress() const {
    return address;
}

void VirtualHole::setAddress(long address) {
    VirtualHole::address = address;
}

long VirtualHole::getSize() const {
    return size;
}

void VirtualHole::setSize(long size) {
    VirtualHole::size = size;
}
