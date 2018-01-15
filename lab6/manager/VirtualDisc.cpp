//
// Created by jakub on 13.01.18.
//

#include "VirtualDisc.h"

const string VirtualDisc::DISC_EXTENSION = ".vd";
const string VirtualDisc::SUFFIX = "#";
const long VirtualDisc::MAX_DISC_SIZE = 1024 * 100;

const string &VirtualDisc::getDiscName() const {
    return discName;
}

void VirtualDisc::setDiscName(const string &discName) {
    VirtualDisc::discName = discName;
}

VirtualDisc::VirtualDisc(const string &discName) :
        discName(discName)
{
    VirtualDisc();
}

VirtualDisc::VirtualDisc()
{
    freeSpace = MAX_DISC_SIZE;
}

long VirtualDisc::getFreeSpace() const {
    return freeSpace;
}

void VirtualDisc::setFreeSpace(long freeSpace) {
    VirtualDisc::freeSpace = freeSpace;
}

const string &VirtualDisc::getDISC_EXTENSION() {
    return DISC_EXTENSION;
}

const long VirtualDisc::getMAX_DISC_SIZE() {
    return MAX_DISC_SIZE;
}
