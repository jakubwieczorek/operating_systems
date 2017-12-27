#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <sys/shm.h>
#include <stdbool.h>
#include "semaphores.h"

struct common_properties m_common_properties = {5, 10, 4};

void build_elem(factory** a_factory)
{
    while(true)
    {
        printf("Helo3");
        lock_queue(a_factory); // down
        if((*(*a_factory)->cur_elem_size)++ >= m_common_properties.elem_amount)
        {
            unlock_queue(a_factory); // up
            detach_shared_data();
            break;
        }

        do_operation((*a_factory)->sem, OVERFLOW_PROTECTOR, -1);
        printf("%d elem creation of type %c\n", 3, (*a_factory)->factory_type);
        enqueue(3, &((*a_factory)->tail), &((*a_factory)->head));
        //sleep(1);
        do_operation((*a_factory)->sem, OVERFLOW_PROTECTOR, 1);
        unlock_queue(a_factory); // up
    }
}

int get_elem(int a_robot_num)
{
    while(true)
    {
        if((*(p_factory)->cur_elem_size)++ >= m_common_properties.elem_amount)
        {
//                    unlock_queue(a_factory); // up
//                    detach_shared_data();
            break;
        }
        printf("aaa\n");
        // y
        do_operation((y_factory)->sem, ZERO_PROTECTOR, -1); // nie w sekcji krytycznej by nie dopuscic do blokady
        lock_queue(&y_factory); // down
        p_factory->p_robots[a_robot_num].y_elem = dequeue(&((y_factory)->head))->elem;
        printf("get elem %d\n", p_factory->p_robots[a_robot_num].y_elem);
        //sleep(1);
        unlock_queue(&y_factory); // up
        do_operation((y_factory)->sem, OVERFLOW_PROTECTOR, 1);

        printf("bbb\n");

        //z
        do_operation((z_factory)->sem, ZERO_PROTECTOR, -1); // nie w sekcji krytycznej by nie dopuscic do blokady
        lock_queue(&z_factory); // down
        p_factory->p_robots[a_robot_num].z_elem = dequeue(&((z_factory)->head))->elem;
        printf("get elem %d\n", p_factory->p_robots[a_robot_num].z_elem);
        //sleep(1);
        unlock_queue(&z_factory); // up
        do_operation((z_factory)->sem, OVERFLOW_PROTECTOR, 1);
        printf("ccc\n");
        fprintf(stdout, "element built from y = %d and z = %d result = %d",
                p_factory->p_robots[a_robot_num].y_elem, p_factory->p_robots[a_robot_num].z_elem,
                p_factory->p_robots[a_robot_num].y_elem * 10 + p_factory->p_robots[a_robot_num].z_elem);
    }

}

void lock_queue(factory **a_factory)
{
    do_operation((*a_factory)->sem, LOCK, -1);
}

void unlock_queue(factory **a_factory)
{
    do_operation((*a_factory)->sem, LOCK, 1);
}

void do_operation(int a_sem, unsigned short a_sem_num, short a_sem_op)
{
    sem_buf->sem_flg = 1;
    sem_buf->sem_num = a_sem_num;
    sem_buf->sem_op = a_sem_op;

    if(semop(a_sem, sem_buf, 1) == -1)
    {
        perror("semop error");
        exit(-1);
    }
}

void init_sem(int a_sem_id, int sem_num, short a_value)
{
    if(semctl(a_sem_id, sem_num, SETVAL, a_value) == -1)
    {
        perror("semctl error");
        exit(-1);
    }
}

void get_shared_data()
{
    if(y_factory->tail != NULL && y_factory->head != NULL)
    {
        y_factory->tail = (queue *) attach(y_factory->tail_id);
        y_factory->head = (queue *) attach(y_factory->head_id);
        y_factory->cur_elem_size = (short*) attach(y_factory->cur_elem_size_id);

        z_factory->tail = (queue *) attach(z_factory->tail_id);
        z_factory->head = (queue *) attach(z_factory->head_id);
        z_factory->cur_elem_size = (short*) attach(z_factory->cur_elem_size_id);

        p_factory->cur_elem_size = (short*) attach(p_factory->cur_elem_size_id);
    }
}

void* attach(int a_id)
{
    void* mem_seg;

    if((mem_seg = shmat(a_id, NULL, 0)) == (void*) -1)
    {
        perror("shmat error");
        exit(-1);
    }

    return mem_seg;
}

void detach_shared_data()
{
    detach(y_factory->tail);
    detach(y_factory->head);
    detach(y_factory->cur_elem_size);

    detach(z_factory->tail);
    detach(z_factory->head);
    detach(z_factory->cur_elem_size);

    detach(p_factory->cur_elem_size);
}

void detach(void* a_mem_seg)
{
    if(shmdt(a_mem_seg) == -1)
    {
        perror("shmdt error");
        exit(-1);
    }
}