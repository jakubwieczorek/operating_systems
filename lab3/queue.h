#ifndef LAB3_QUEUE_H
#define LAB3_QUEUE_H

typedef struct queue
{
    struct queue *next;
    int elem;
} queue;

void enqueue(int elem, queue **a_tail, queue **a_head);
queue* dequeue(queue **a_head);

#endif