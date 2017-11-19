#include <stdio.h>
#include <stdbool.h>
#include "proc.h"

struct proc* build_proc(int a_prioroty, char a_name[]);

void main(void)
{
//    ready(build_proc(HIGH, "HIGH"));
//    ready(build_proc(HIGH, "HIGH1"));
//    ready(build_proc(HIGH, "HIGH2"));

    ready(build_proc(MEDIUM, "MEDIUM"));
    ready(build_proc(MEDIUM, "MEDIUM1"));
    ready(build_proc(MEDIUM, "MEDIUM2"));

//    ready(build_proc(LOW, "LOW"));
//    ready(build_proc(LOW, "LOW1"));
//    ready(build_proc(LOW, "LOW2"));

    char c = 0;

    while(1)
    {
        scanf("%c", &c);
        if(c == '\n')
        {
            pick_proc();
            printf("%s\n", bill_ptr->name);
        }
    }
}

struct proc* build_proc(int a_prioroty, char a_name[])
{
    struct proc* new_proc = malloc(sizeof(struct proc));

    int size = sizeof(a_name) / sizeof(char);
    for(int i = 0; i < size; i++)
    {
        new_proc->name[i] = a_name[i];
    }

    new_proc->priority = a_prioroty;
    new_proc->was_used = false;
    return new_proc;
}

