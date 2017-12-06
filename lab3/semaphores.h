#ifndef LAB3_SEMAPHORES_H
#define LAB3_SEMAPHORES_H

#include "queue.h"

typedef struct factory
{
    int robots_num; // robotow produkuje element i kladzie je na buforze by
    char factory_type;

    size_t queue_size;
    queue *head, *tail; // pierwszy, ostatni element w kolejce

    int head_id, tail_id;
    int head_shm_key, tail_shm_key;

    int zero_protector_sem, zero_protector_sem_key;
    int overflow_protector_sem, overflow_protector_sem_key;

    int access_protector_sem, access_protector_sem_key;
} factory;


typedef struct p_robot
{
    int y_elem;
    int z_elem;
} p_robot;

typedef struct p_factory_t
{
    int robots_num;
    int elem_num; // ilosc elementow do stworzenia
    p_robot *p_robots;
} p_factory_t;

p_factory_t *p_factory;
factory *y_factory, *z_factory;
struct sembuf *sem_buf;

void create_elem(factory *a_factory, int a_elem, char a_elem_type);
void do_operation(int a_sem, short a_sem_op);
int get_elem(factory *a_factory);
void lock_queue(factory *a_factory);
void unlock_queue(factory *a_factory);
void get_shared_data();

#endif
