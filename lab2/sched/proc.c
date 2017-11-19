#include "proc.h"

struct proc *bill_ptr;
struct proc *u_rdy_head[U_NQ] = {NIL_PROC, NIL_PROC, NIL_PROC};
struct proc *u_rdy_tail[U_NQ] = {NIL_PROC, NIL_PROC, NIL_PROC};

void pick_proc()
{
    struct proc *rp;

    if((rp = u_rdy_head[HIGH]) != NIL_PROC)
    {
        // gdy kolejny nie byl uzyty i medium i low nie istnieja
        if(rp->was_used == false || (u_rdy_head[MEDIUM] == NIL_PROC && u_rdy_head[LOW] == NIL_PROC))
        {
            bill_ptr = rp;

            swipe_head(&u_rdy_head[HIGH], &u_rdy_tail[HIGH]);

            return;
        }
    }

    if((rp = u_rdy_head[MEDIUM]) != NIL_PROC)
    {
        // gdy kolejny nie byl uzyty lub high i low nie istnieja
        if(rp->was_used == false || (u_rdy_head[HIGH] == NIL_PROC && u_rdy_head[LOW] == NIL_PROC))
        {
            bill_ptr = rp;

            // przelacz
            swipe_head(&u_rdy_head[MEDIUM], &u_rdy_tail[MEDIUM]);

            // wyczysc gdy nie ma low lub kolejny nie ma byc low
            if(u_rdy_head[MEDIUM]->was_used == false || u_rdy_tail[LOW] == NIL_PROC)
            {
                clear_used(&u_rdy_head[HIGH]);
            }

            return;
        }
    }

    if((rp = u_rdy_head[LOW]) != NIL_PROC)
    {
        bill_ptr = rp;

        swipe_head(&u_rdy_head[LOW], &u_rdy_tail[LOW]);

        clear_used(&u_rdy_head[MEDIUM]);
        clear_used(&u_rdy_head[HIGH]);

        return;
    }
}

void swipe_head(struct proc** a_head, struct proc** a_tail)
{
    /*  tail i head sa zapetlone czyli:
     *
     *      0->1->2->0->1->2-> ...
     *  head^     ^tail ...
     * */
    (*a_tail)->p_nextready = *a_head;

    /*  tail i head sa rowne i zapetlone czyli:
     *
     *      0->1->2->0->1->2-> ...
     *  head^tail ...
     * */
    *a_tail = *a_head;

    /*  head jest bylym drugim w kolejce i zapetlonym czyli
     *
     *      0->1->2->0->1->2-> ...
     *     head^     ^tail ...
     * */
    *a_head = (*a_head)->p_nextready;

    /*  head jest bylym drugim w kolejce i zapetlonym czyli
     *
     *         1->2->0
     *     head^     ^tail
     * */
    (*a_tail)->p_nextready = NIL_PROC;

    (*a_tail)->was_used = true;
}

void clear_used(struct proc** a_head)
{
    for(struct proc* head = *a_head; head != NIL_PROC; head = head->p_nextready)
    {
        head->was_used = false;
    }
}

void ready(struct proc* rp)
{
    put(rp);
}

void put(struct proc* rp)
{
    int priority = rp->priority;

    if (u_rdy_head[priority] == NIL_PROC)
        u_rdy_tail[priority] = rp;
    rp->p_nextready = u_rdy_head[priority];
    u_rdy_head[priority] = rp;
}