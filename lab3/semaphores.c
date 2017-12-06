#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <sys/shm.h>
#include "semaphores.h"

void create_elem(factory *a_factory, int a_elem, char a_elem_type)
{
    lock_queue(a_factory); // down
    printf("%d elem creation of type %c\n", a_elem, a_elem_type);
    enqueue(a_elem, &(a_factory->tail), &(a_factory->head));
    sleep(1);
    do_operation(a_factory->overflow_protector_sem, -1);
    do_operation(a_factory->zero_protector_sem, 1);
    unlock_queue(a_factory); // up
}

void lock_queue(factory *a_factory)
{
    do_operation(a_factory->access_protector_sem, -1);
}

void unlock_queue(factory *a_factory)
{
    do_operation(a_factory->access_protector_sem, 1);
}

int get_elem(factory *a_factory)
{
    do_operation(a_factory->zero_protector_sem, -1); // nie w sekcji krytycznej by nie dopuscic do blokady
    lock_queue(a_factory); // down
    int elem = dequeue(&(a_factory->head))->elem;
    printf("get elem %d\n", elem);
    sleep(1);
    unlock_queue(a_factory); // up
    do_operation(a_factory->overflow_protector_sem, 1);

    return elem;
}

void do_operation(int a_sem, short a_sem_op)
{
    sem_buf->sem_flg = 1;
    sem_buf->sem_num = 0;
    sem_buf->sem_op = a_sem_op;

    if(semop(a_sem, sem_buf, 1) == -1)
    {
        perror("semop error");
        exit(-1);
    }
}

void get_shared_data()
{
    if(y_factory->tail != NULL && y_factory->head != NULL)
    {
        y_factory->tail = (queue *) shmat(y_factory->tail_id, NULL, SHM_EXEC);
        y_factory->head = (queue *) shmat(y_factory->head_id, NULL, SHM_EXEC);

        z_factory->tail = (queue *) shmat(z_factory->tail_id, NULL, SHM_EXEC);
        z_factory->head = (queue *) shmat(z_factory->head_id, NULL, SHM_EXEC);
    }
}