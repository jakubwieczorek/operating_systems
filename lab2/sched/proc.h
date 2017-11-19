#ifndef PROC_H
#define PROC_H

#include <stdlib.h>
#include <stdbool.h>
#include "const.h"

struct proc
{
    struct proc *p_nextready;
    int priority;
    bool was_used;
    char name[20];
};

#define NIL_PROC          ((struct proc *) 0)

extern struct proc *bill_ptr;
extern struct proc *u_rdy_head[U_NQ];
extern struct proc *u_rdy_tail[U_NQ];

void pick_proc();
void swipe_head(struct proc** a_head, struct proc** a_tail);
void clear_used(struct proc** a_head);
void ready(struct proc* rp);
void put(struct proc* rp);

#endif /* PROC_H */
