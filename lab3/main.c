#include <zconf.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include "queue.h"
#include "main.h"
#include "semaphores.h"

int main(int argc, char *argv[])
{
    init(argc, argv);
    init_shared_data();
    for(int i = 0; i < y_factory->robots_num; i++)
    {
        if(fork() == 0); // child
        {
            get_shared_data();
            create_elem(y_factory, 3, y_factory->factory_type);
        }
    }

    for(int i = 0; i < z_factory->robots_num; i++)
    {
        if(fork() == 0); // child
        {
            get_shared_data();
            create_elem(z_factory, 6, z_factory->factory_type);
        }
    }

    for(int i = 0; i < p_factory->robots_num; i++)
    {
        if(fork() == 0); // child
        p_factory->p_robots[i].y_elem = get_elem(y_factory);
        p_factory->p_robots[i].z_elem = get_elem(z_factory);
        fprintf(stdout, "element built from y = %d and z = %d result = %d",
                p_factory->p_robots[i].y_elem, p_factory->p_robots->z_elem,
                p_factory->p_robots[i].y_elem * 10 + p_factory->p_robots->z_elem);
    }

    return 0;
}

void init(int argc, char *argv[])
{
    key_t key = 1234;

    y_factory = malloc(sizeof(factory));
    y_factory->robots_num = 1;
    y_factory->factory_type = 'Y';
    y_factory->tail = NULL;
    y_factory->head = NULL;
    y_factory->queue_size = 5;
    y_factory->head_shm_key = key++;
    y_factory->tail_shm_key = key++;
    y_factory->access_protector_sem_key = key++;
    y_factory->zero_protector_sem_key = key++;
    y_factory->overflow_protector_sem_key = key++;

    z_factory = malloc(sizeof(factory));
    z_factory->robots_num = 1;
    z_factory->factory_type = 'Z';
    z_factory->queue_size = 5;
    z_factory->tail = NULL;
    z_factory->head = NULL;
    z_factory->head_shm_key = key++;
    z_factory->tail_shm_key = key++;
    z_factory->access_protector_sem_key = key++;
    z_factory->zero_protector_sem_key = key++;
    z_factory->overflow_protector_sem_key = key;

    p_factory = malloc(sizeof(p_factory_t));
    p_factory->elem_num = 10;
    p_factory->robots_num = 1;
    p_factory->p_robots = malloc(p_factory->robots_num * sizeof(p_robot));
}

void init_shared_data()
{
    sem_buf = malloc(sizeof(struct sembuf)); // argument in each semop operation

    // kolejka pamiec wspoldzielona
    y_factory->head_id = shmget(y_factory->head_shm_key, sizeof(queue), 0666 | IPC_CREAT);
    y_factory->tail_id = shmget(y_factory->tail_shm_key, sizeof(queue), 0666 | IPC_CREAT);

    // semafory
    y_factory->access_protector_sem = build_sem(y_factory->access_protector_sem_key, 1);
    y_factory->overflow_protector_sem = build_sem(y_factory->overflow_protector_sem_key, 1);
    y_factory->zero_protector_sem = build_sem(y_factory->zero_protector_sem_key, 1);

    // init
    do_operation(y_factory->access_protector_sem, 1);
    //do_operation(y_factory->zero_protector_sem, 0);
    do_operation(y_factory->overflow_protector_sem, (short) y_factory->queue_size);

    z_factory->head_id = shmget(z_factory->head_shm_key, sizeof(queue), 0666 | IPC_CREAT);
    z_factory->tail_id = shmget(z_factory->tail_shm_key, sizeof(queue), 0666 | IPC_CREAT);

    z_factory->access_protector_sem = build_sem(z_factory->access_protector_sem_key, 1);
    z_factory->overflow_protector_sem = build_sem(z_factory->overflow_protector_sem_key, 1);
    z_factory->zero_protector_sem = build_sem(z_factory->zero_protector_sem_key, 1);

    // init
    do_operation(z_factory->access_protector_sem, 1);
    //do_operation(z_factory->zero_protector_sem, 0);
    do_operation(z_factory->overflow_protector_sem, (short) z_factory->queue_size);
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