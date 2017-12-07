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

    create_robots(&y_factory);
    create_robots(&z_factory);

    __pid_t pid;

    for(int i = 0; i < p_factory->robots_num; i++)
    {
        if((pid = fork()) == 0)
        {
            get_shared_data();

            printf("dziecko robot p\n");

            for(int elem = 0; elem < p_factory->elem_num; elem++)
            {
                printf("aaa\n");
                p_factory->p_robots[i].y_elem = get_elem(&y_factory);
                printf("bbb\n");
                p_factory->p_robots[i].z_elem = get_elem(&z_factory);
                printf("ccc\n");
                fprintf(stdout, "element built from y = %d and z = %d result = %d",
                        p_factory->p_robots[i].y_elem, p_factory->p_robots[i].z_elem,
                        p_factory->p_robots[i].y_elem * 10 + p_factory->p_robots[i].z_elem);
            }

            detach_shared_data();
        } else if(pid == -1)
        {
            perror("fork error");
            exit(-1);
        }
    }

    return 0;
}

void create_robots(factory** a_factory)
{
    __pid_t pid;

    for(int i = 0; i < (*a_factory)->robots_num; i++)
    {
        if((pid = fork()) == 0) // child
        {
            get_shared_data();

            for(int elem = 0; elem < p_factory->elem_num; elem++)
            {
                create_elem(a_factory, 3, (*a_factory)->factory_type);
            }

            detach_shared_data();
        } else if(pid == -1)
        {
            perror("fork error");
            exit(-1);
        }
    }
}

void init(int argc, char *argv[])
{
    init_factory(&y_factory, 'Y', 3);
    init_factory(&z_factory, 'Z', 3);

    p_factory = malloc(sizeof(p_factory_t));
    p_factory->elem_num = 10;
    p_factory->robots_num = 3;
    p_factory->p_robots = malloc(p_factory->robots_num * sizeof(p_robot));
}

void init_factory(factory** a_factory, char a_type, int a_robots_num)
{
    static key_t key = 1234;

    *a_factory = malloc(sizeof(factory));
    (*a_factory)->robots_num = a_robots_num;
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
    init_sem(y_factory->access_protector_sem, 1);
    init_sem(y_factory->zero_protector_sem, 0);
    init_sem(y_factory->overflow_protector_sem, (short) y_factory->queue_size);

    z_factory->head_id = build_shm_id(z_factory->head_shm_key, sizeof(queue));
    z_factory->tail_id = build_shm_id(z_factory->tail_shm_key, sizeof(queue));

    z_factory->access_protector_sem = build_sem(z_factory->access_protector_sem_key, 1);
    z_factory->overflow_protector_sem = build_sem(z_factory->overflow_protector_sem_key, 1);
    z_factory->zero_protector_sem = build_sem(z_factory->zero_protector_sem_key, 1);

    init_sem(z_factory->access_protector_sem, 1);
    init_sem(z_factory->zero_protector_sem, 0);
    init_sem(z_factory->overflow_protector_sem, (short) z_factory->queue_size);
}

int build_shm_id(int a_key, size_t a_size)
{
    int shm;

    if((shm = shmget(a_key, a_size, 0666 | IPC_CREAT)) == -1)
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