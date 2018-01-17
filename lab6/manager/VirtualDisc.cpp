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

long VirtualDisc::getDiscSize() const {
    return discSize;
}

void VirtualDisc::setDiscSize(long discSize) {
    VirtualDisc::discSize = discSize;
}

const list <VirtualFile> &VirtualDisc::getFiles() const {
    return files;
}

void VirtualDisc::setFiles(const list <VirtualFile> &files) {
    VirtualDisc::files = files;
}

const list <VirtualHole> &VirtualDisc::getHoles() const {
    return holes;
}

void VirtualDisc::setHoles(const list <VirtualHole> &holes) {
    VirtualDisc::holes = holes;
}

const string &VirtualDisc::getSUFFIX() {
    return SUFFIX;
}
