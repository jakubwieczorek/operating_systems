//
// Created by jakub on 15.01.18.
//

#include "VirtualFile.h"

const string VirtualFile::DESCRIPTOR = "FILE";
const string VirtualFile::SUFFIX = ")";
const string VirtualFile::PREFIX = "(";

long VirtualFile::getAddress() const {
    return address;
}

void VirtualFile::setAddress(long address) {
    VirtualFile::address = address;
}

long VirtualFile::getSize() const {
    return size;
}

void VirtualFile::setSize(long size) {
    VirtualFile::size = size;
}

const string &VirtualFile::getName() const {
    return name;
}

void VirtualFile::setName(const string &name) {
    VirtualFile::name = name;
}

const string &VirtualFile::getContent() const {
    return content;
}

void VirtualFile::setContent(const string &content) {
    VirtualFile::content = content;
}

bool VirtualFile::operator==(const VirtualFile &rhs) const {
    return name == rhs.name;
}

bool VirtualFile::operator!=(const VirtualFile &rhs) const {
    return !(rhs == *this);
}
