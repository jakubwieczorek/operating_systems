#include <stdlib.h>
#include "queue.h"

void enqueue(int elem, queue **a_tail, queue **a_head)
{
    queue *new_elem = malloc(sizeof(queue));
    new_elem->elem = elem;

    if(*a_tail == NULL)
    {
        *a_tail = new_elem;
        *a_head = new_elem;
        return;
    }

    (*a_tail)->next = new_elem;
    (*a_tail) = (*a_tail)->next;
}

queue* dequeue(queue **a_head)
{
    if(*a_head == NULL)
    {
        return NULL;
    }

    queue *to_return = *a_head;
    *a_head = (*a_head)->next;

    return to_return;
}
