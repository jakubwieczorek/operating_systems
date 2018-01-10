//
// Created by jakub on 09.01.18.
//

#ifndef MANAGER_MAIN_H
#define MANAGER_MAIN_H
long check(const char *str, int n, ...);//n ilosc dodatkowych argumentow, pierwszy dodatkowy to  min drugi max.
void menu();
void str(string &name, const char *msg, int a_size);
void virtual_disk_menu(MemoryManager aManager);
#endif //MANAGER_MAIN_H
