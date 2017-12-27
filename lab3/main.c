#include <zconf.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdbool.h>
#include "queue.h"
#include "main.h"

extern struct common_properties m_common_properties;

int main(int argc, char *argv[])
{
    init(argc, argv);

    create_robots(&y_factory);
    create_robots(&z_factory);

    __pid_t pid;

    for(int i = 0; i < p_factory->robots_num; i++)
    {
        if((pid = fork()) == 0)
        {
            get_shared_data();

            printf("dziecko robot p\n");

            get_elem(i);

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
        printf("Helo");

        if((pid = fork()) == 0) // child
        {
            printf("Helo1");
            get_shared_data();

            build_elem(a_factory);

            printf("Helo2");

        } else if(pid == -1)
        {
            perror("fork error");
            exit(-1);
        }
    }
}

void init(int argc, char *argv[])
{
    init_factory(&y_factory, 'Y', 3, argv[0]);
    init_factory(&z_factory, 'Z', 3, argv[0]);

    sem_buf = malloc(sizeof(struct sembuf)); // argument in each semop operation

    p_factory = malloc(sizeof(p_factory_t));
    p_factory->robots_num = 3;
    p_factory->p_robots = malloc(p_factory->robots_num * sizeof(p_robot));
    p_factory->cur_elem_shm_key = ftok(argv[0], 'm');

    p_factory->cur_elem_size_id = build_shm_id(p_factory->cur_elem_shm_key, sizeof(short));
}

void init_factory(factory** a_factory, char a_type, int a_robots_num, char *a_path)
{
    static char key = '*';

    *a_factory = malloc(sizeof(factory));
    (*a_factory)->robots_num = a_robots_num;
    (*a_factory)->cur_elem_size = 0;
    (*a_factory)->factory_type = a_type;
    (*a_factory)->tail = NULL;
    (*a_factory)->head = NULL;
    (*a_factory)->head_shm_key = ftok(a_path, key++);
    (*a_factory)->tail_shm_key = ftok(a_path, key++);
    (*a_factory)->sem_key = ftok(a_path, key++);
    (*a_factory)->cur_elem_shm_key = ftok(a_path, key++);

    // kolejka pamiec wspoldzielona
    (*a_factory)->head_id = build_shm_id((*a_factory)->head_shm_key, sizeof(queue));
    (*a_factory)->tail_id = build_shm_id((*a_factory)->tail_shm_key, sizeof(queue));
    (*a_factory)->cur_elem_size_id = build_shm_id((*a_factory)->cur_elem_shm_key, sizeof(short));

    // semafory
    (*a_factory)->sem = build_sem((*a_factory)->sem_key, m_common_properties.sem_size);

    // init
    init_sem((*a_factory)->sem, LOCK, 1);
    init_sem((*a_factory)->sem, ZERO_PROTECTOR, 0);
    init_sem((*a_factory)->sem, OVERFLOW_PROTECTOR, (short) m_common_properties.queue_size);
    init_sem((*a_factory)->sem, ELEM_NUMBER_PROTECTOR, m_common_properties.elem_amount);
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
