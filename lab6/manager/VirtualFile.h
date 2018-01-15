//
// Created by jakub on 15.01.18.
//

#ifndef MANAGER_VIRTUALFILE_H
#define MANAGER_VIRTUALFILE_H
#include<iostream>

using namespace std;

class VirtualFile {
private:
    long address;
    long size;
    string name;
    string content;
public:
    static const string DESCRIPTOR;
    static const string SUFFIX;
    static const string PREFIX;

    long getAddress() const;

    void setAddress(long address);

    long getSize() const;

    void setSize(long size);

    const string &getName() const;

    void setName(const string &name);

    const string &getContent() const;

    void setContent(const string &content);
};


#endif //MANAGER_VIRTUALFILE_H
