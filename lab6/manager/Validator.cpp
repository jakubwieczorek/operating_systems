//
// Created by jakub on 13.01.18.
//

#include "Validator.h"

using namespace std;

string Validator::str(const char *msg, int a_size)
{
    string str;

    while(true)
    {
        cout<<msg;
        cin>>str;
        if(str.size() <= a_size && str.size() >= 1)
        {
            break;
        }
        continue;
    }

    return str;
}

long Validator::check(const char *str, int n, ...)//n ilosc dodatkowych argumentow, pierwszy dod min drugi max.
{
    long to_check;

    va_list ap;//tworzy zmienna ap typu va_list
    va_start(ap, n);//udostępnia parametry znajdujące się po parametrze n

    int min, max; //zmienne potrzebne do tego, że va_arg po każdym wywołaniu przesówa wskaźnik o jeden więc w przypadku pomyłki jest
    //problem

    min=va_arg(ap, int);
    if(--n) max=va_arg(ap, int);// gdy chcemy ograniczyc z gory

    while(true)
    {
        cin.clear();

        cout<<str;//wyswietla wiadomosc z parametru
        cin>>to_check;//pobiera wartosc do sprawdzenia

        if(cin.fail())//jesli blad wprowadzenia
        {
            while(fgetc(stdin)!='\n');//czyszczenie bufora
            cin.clear();
            continue;
        }

        if(to_check<=min)
        {continue;}//gdy jest mniejsze od minimum

        if(n)
            if(to_check>=max)
            {continue;}//gdy jest wieksze od max

        break;
    }
    va_end(ap);
    while(cin.get()!='\n');//czyszczenie bufora
    cin.clear();

    return to_check;
}