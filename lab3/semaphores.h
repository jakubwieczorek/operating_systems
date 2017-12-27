#ifndef LAB3_SEMAPHORES_H
#define LAB3_SEMAPHORES_H

#include "queue.h"

enum {ZERO_PROTECTOR, OVERFLOW_PROTECTOR, LOCK, ELEM_NUMBER_PROTECTOR};

struct common_properties
{
    const size_t queue_size;
    const short elem_amount;
    const int sem_size;
};

typedef struct factory
{
    int robots_num; // robotow produkuje element i kladzie je na buforze by
    char factory_type;

    queue *head, *tail; // pierwszy, ostatni element w kolejce
    short* cur_elem_size;

    int head_id, tail_id;
    int head_shm_key, tail_shm_key;
    int cur_elem_size_id, cur_elem_shm_key;

    int sem_key, sem;

} factory;

typedef struct p_robot
{
    int y_elem;
    int z_elem;
} p_robot;

typedef struct p_factory_t
{
    int robots_num;
    p_robot *p_robots;

    short* cur_elem_size;
    int cur_elem_size_id, cur_elem_shm_key;
} p_factory_t;

p_factory_t *p_factory;
factory *y_factory, *z_factory;
struct sembuf *sem_buf;

void do_operation(int a_sem, unsigned short a_sem_num, short a_sem_op);
int get_elem(int a_robot_num);
void lock_queue(factory **a_factory);
void unlock_queue(factory **a_factory);
void get_shared_data();
void detach(void* a_mem_seg);
void detach_shared_data();
void* attach(int a_id);
void init_sem(int a_sem_id, int sem_num, short a_value);
void build_elem(factory** a_factory);

#endif