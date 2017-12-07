#include <zconf.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include "queue.h"
#include "main.h"

int main(int argc, char *argv[])
{
    init(argc, argv);
    init_shared_data();

    for(int i = 0; i < y_factory->robots_num; i++)
    {
        if(fork() == 0); // child
        {
            get_shared_data();

            for(int elem = 0; elem < p_factory->elem_num; elem++)
            {
                create_elem(y_factory, 3, y_factory->factory_type);
            }

            detach_shared_data();
        }
    }

    for(int i = 0; i < z_factory->robots_num; i++)
    {
        if(fork() == 0) // child
        {
            get_shared_data();

            for(int elem = 0; elem < p_factory->elem_num; elem++)
            {
                create_elem(z_factory, 6, z_factory->factory_type);
            }

            detach_shared_data();
        }
    }

    for(int i = 0; i < p_factory->robots_num; i++)
    {
        if(fork() == 0)
        {
            for(int elem = 0; elem < p_factory->elem_num; elem++)
            {
                p_factory->p_robots[i].y_elem = get_elem(y_factory);
                p_factory->p_robots[i].z_elem = get_elem(z_factory);
                fprintf(stdout, "element built from y = %d and z = %d result = %d",
                        p_factory->p_robots[i].y_elem, p_factory->p_robots->z_elem,
                        p_factory->p_robots[i].y_elem * 10 + p_factory->p_robots->z_elem);
            }
        }// child
    }

    return 0;
}

void init(int argc, char *argv[])
{
    init_factory(&y_factory, 'Y', 5);
    init_factory(&z_factory, 'Z', 5);

    p_factory = malloc(sizeof(p_factory_t));
    p_factory->elem_num = 10;
    p_factory->robots_num = 1;
    p_factory->p_robots = malloc(p_factory->robots_num * sizeof(p_robot));
}

void init_factory(factory** a_factory, char a_type, int a_robots_num)
{
    static key_t key = 1234;

    *a_factory = malloc(sizeof(factory));
    (*a_factory)->robots_num =a_robots_num;
    (*a_factory)->factory_type = a_type;
    (*a_factory)->tail = NULL;
    (*a_factory)->head = NULL;
    (*a_factory)->queue_size = 5;
    (*a_factory)->head_shm_key = key++;
    (*a_factory)->tail_shm_key = key++;
    (*a_factory)->access_protector_sem_key = key++;
    (*a_factory)->zero_protector_sem_key = key++;
    (*a_factory)->overflow_protector_sem_key = key++;
}

void init_shared_data()
{
    sem_buf = malloc(sizeof(struct sembuf)); // argument in each semop operation

    // kolejka pamiec wspoldzielona
    y_factory->head_id = build_shm_id(y_factory->head_shm_key, sizeof(queue));
    y_factory->tail_id = build_shm_id(y_factory->tail_shm_key, sizeof(queue));

    // semafory
    y_factory->access_protector_sem = build_sem(y_factory->access_protector_sem_key, 1);
    y_factory->overflow_protector_sem = build_sem(y_factory->overflow_protector_sem_key, 1);
    y_factory->zero_protector_sem = build_sem(y_factory->zero_protector_sem_key, 1);

    // init
    do_operation(y_factory->access_protector_sem, 1);
    //do_operation(y_factory->zero_protector_sem, 0);
    do_operation(y_factory->overflow_protector_sem, (short) y_factory->queue_size);

    z_factory->head_id = build_shm_id(z_factory->head_shm_key, sizeof(queue));
    z_factory->tail_id = build_shm_id(z_factory->tail_shm_key, sizeof(queue));

    z_factory->access_protector_sem = build_sem(z_factory->access_protector_sem_key, 1);
    z_factory->overflow_protector_sem = build_sem(z_factory->overflow_protector_sem_key, 1);
    z_factory->zero_protector_sem = build_sem(z_factory->zero_protector_sem_key, 1);

    do_operation(z_factory->access_protector_sem, 1);
    //do_operation(z_factory->zero_protector_sem, 0);
    do_operation(z_factory->overflow_protector_sem, (short) z_factory->queue_size);
}

int build_shm_id(int a_key, int a_size)
{
    int shm;

    if((shm = shmget(y_factory->head_shm_key, sizeof(queue), 0666 | IPC_CREAT)) == -1)
    {
        perror("shmget error");
        exit(-1);
    }

    return shm;
}

int build_sem(int a_key, int a_size)
{
    int sem;

    if((sem = semget(a_key, a_size, 0666 | IPC_CREAT)) == -1)
    {
        perror("semget error");
        exit(-1);
    }

    return sem;
}