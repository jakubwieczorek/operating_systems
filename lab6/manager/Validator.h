//
// Created by jakub on 13.01.18.
//

#ifndef MANAGER_VALIDATOR_H
#define MANAGER_VALIDATOR_H

#include<iostream>
#include<fstream>
#include<cstdarg>

using namespace std;

class Validator {

public:
    static string str(const char *msg, int a_size);
    static long check(const char *str, int n, ...);//n ilosc dodatkowych argumentow, pierwszy dod min drugi max.
};


#endif //MANAGER_VALIDATOR_H
