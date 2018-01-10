#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "main.h"

int main(int argc, char** argv)
{
    if(argc != 7)
    {
        printf("Nieprawidlowa liczba argumentow\nSprobuj ponownie podajac wartosc M N P R S T\n");
        return 1;
    }

    z_factory.robots_number = atoi(argv[1]);
    y_factory.robots_number = atoi(argv[2]);
    P=atoi(argv[3]);
    y_factory.max_work_time = atoi(argv[4]);
    z_factory.max_work_time = atoi(argv[5]);
    T = atoi(argv[6]);

    srand(time(NULL));

    k4=ftok(".", 'D');
    end_protector_sem = semget(k4, 1, IPC_CREAT | 0600);

    init_factory(&y_factory);
    init_factory(&z_factory);

    semctl(end_protector_sem, 0, SETVAL, 1);

    k3=ftok(".", 'C');
    end_counter_shm = shmget(k3, sizeof(int), IPC_CREAT | 0600);
    process_counter = (int*)shmat(end_counter_shm, NULL, 0);
    (*process_counter) = y_factory.robots_number + z_factory.robots_number + P;

    // Tworzenie robotow:
    create_robots(&y_factory);
    create_robots(&z_factory);

    for(int i=0; i < P; i++)
    {
        // Utworzenie nowego procesu
        if(fork() == 0)
        {
            // Podlaczenie pamieci wspoldzielonej
            y_factory.buffer = (int*)shmat(y_factory.mem_id, NULL, 0);
            z_factory.buffer = (int*)shmat(z_factory.mem_id, NULL, 0);

            consume_elements();
        }
    }

    return 0;
}

void init_factory(struct producer_factory* a_factory)
{
    static int key_id = 999;

    // Utworzenie semafor�w
    a_factory->sem_mem_key = ftok(".", key_id++);
    a_factory->semaphores = semget(a_factory->sem_mem_key, 4, IPC_CREAT | 0600);

    // Nadanie wartosci poczatkowej
    semctl(a_factory->semaphores, WRITE_MTX, SETVAL, 1);
    semctl(a_factory->semaphores, READ_MTX, SETVAL, 1);
    semctl(a_factory->semaphores, FREE_COUNTER, SETVAL, (int)BUFFER_SIZE);
    semctl(a_factory->semaphores, BUSY_COUNTER, SETVAL, 0);

    // Przydzielenie pamieci wspoldzielonej
    // bufor, indeksy do zapisu i odczytu liczniki elementow;
    a_factory->mem_id = shmget(a_factory->sem_mem_key, (BUFFER_SIZE + 4) * sizeof(int), IPC_CREAT | 0600);
}

void create_robots(struct producer_factory* a_factory)
{
    for(int i=0; i < a_factory->robots_number; i++)
    {
        // create new process and if child:
        if(fork() == 0)
        {
            // get shared memory
            a_factory->buffer = (int*) shmat(a_factory->mem_id, NULL, 0);

            if(i == 0)
            {
                a_factory->buffer[BUF_BUSY_COUNTER] = a_factory->buffer[BUF_FREE_COUNTER] = 0;
                a_factory->buffer[BUF_IDX_WRITE] = a_factory->buffer[BUF_IDX_READ] = 0;
            }

            create_elements(a_factory);
        }
    }
}

void create_elements(struct producer_factory* a_factory)
{
    while(1)
    {
        // zmniejsz ilosc wolnych miejsc
        down(a_factory->semaphores, FREE_COUNTER);

        // zablokuj mozliwosc pisania do kolejki dla innych robotow
        lock(a_factory->semaphores, WRITE_MTX);

        if(a_factory->buffer[BUF_BUSY_COUNTER] < 10)
        {
            sleep(rand() % y_factory.max_work_time + 1);

#define idx_write a_factory->buffer[BUF_IDX_WRITE]
            // tworzone najpierw 0, 1, 2 itd az do 10 zeby bylo prosciej
            a_factory->buffer[idx_write] = a_factory->buffer[BUF_BUSY_COUNTER];

            // pisz na kolejnym elemencie, jezeli doszedles do konca kolejki to zacznij pisac na poczatku
            idx_write = (idx_write + 1) % BUFFER_SIZE;

            // zwieksz licznik elementow
            a_factory->buffer[BUF_BUSY_COUNTER]++;
        }

        // odblokuj mozliwosc pisania dla innych
        unlock(a_factory->semaphores, WRITE_MTX);

        // zwieksz ilosc zajetych miejsc
        up(a_factory->semaphores, BUSY_COUNTER);

        // zakoncz prace gdy wytworzono dostateczna ilosc elementow
        if(a_factory->buffer[BUF_BUSY_COUNTER] == 10)
        {
            break;
        }
    }

    // usun semafory
    clear_factory(a_factory);
}

void clear_factory(struct producer_factory* a_factory)
{
    lock(end_protector_sem, 0);
    (*process_counter)--;
    shmdt(a_factory->buffer);

    if((*process_counter) == 0)
    {
        shmdt(process_counter);
        semctl(end_protector_sem, 0, IPC_RMID);
        exit(0) ;
    }

    unlock(end_protector_sem, 0);
    exit(0);
}

int consume_element(struct producer_factory* a_factory)
{
    int elem;

    // zmniejszenie ilosci zajetych miejsc
    down(a_factory->semaphores, BUSY_COUNTER);
    lock(a_factory->semaphores, READ_MTX);

    if(a_factory->buffer[BUF_FREE_COUNTER] < 10)
    {
        // pobierz element
        elem = a_factory->buffer[a_factory->buffer[BUF_IDX_READ]];
        // zmien miejsce czytania
        a_factory->buffer[BUF_IDX_READ] = (a_factory->buffer[BUF_IDX_READ] + 1) % BUFFER_SIZE;
        //  zwieksz ilosc wolnych miejsc
        a_factory->buffer[BUF_FREE_COUNTER]++;
        up(a_factory->semaphores, FREE_COUNTER);
    }

    return elem;
}

void consume_elements()
{
    int first_elem, second_elem;

    while(1)
    {
        first_elem = consume_element(&y_factory);
        unlock(y_factory.semaphores, READ_MTX);

        // zmniejszenie ilosci zajetych miejsc
        down(z_factory.semaphores, BUSY_COUNTER);
        lock(z_factory.semaphores, READ_MTX);

        if(z_factory.buffer[BUF_FREE_COUNTER] < 10)
        {
            // pobierz element
            second_elem = z_factory.buffer[z_factory.buffer[BUF_IDX_READ]];
            // zmien miejsce czytania
            z_factory.buffer[BUF_IDX_READ] = (z_factory.buffer[BUF_IDX_READ] + 1) % BUFFER_SIZE;
            //  zwieksz ilosc wolnych miejsc
            z_factory.buffer[BUF_FREE_COUNTER]++;
            up(z_factory.semaphores, FREE_COUNTER);

            sleep(rand() % T + 1);
            printf("Created element: %d%d\n", first_elem, second_elem);
        }

        unlock(z_factory.semaphores, READ_MTX);

        if(z_factory.buffer[BUF_FREE_COUNTER] == 10)
        {
            break;
        }
    }

    lock(end_protector_sem, 0);
    --(*process_counter);

    delete_shm_sem(&y_factory);
    delete_shm_sem(&z_factory);

    shmctl(end_counter_shm, IPC_RMID, (struct shmid_ds* )0);

    if((*process_counter)==0)
    {
    	shmdt(process_counter);
        semctl(end_protector_sem, 0, IPC_RMID);
	    exit(0);
    }
    unlock(end_protector_sem, 0);
    exit(0);
}

void delete_shm_sem(struct producer_factory* a_factory)
{
    // usuniecie pamieci wspoldzielonej
    shmctl(a_factory->mem_id, IPC_RMID, (struct shmid_ds*)0);
    shmdt(a_factory->buffer);
    semctl(a_factory->semaphores, 0, IPC_RMID);
}

void unlock(int a_semid, int a_semnum)
{
    do_sem_operation(a_semid, a_semnum, 1);
}

void lock(int a_semid, int a_semnum)
{
    do_sem_operation(a_semid, a_semnum, -1);
}

void up(int a_semid, int a_semnum)
{
    do_sem_operation(a_semid, a_semnum, 1);
}

void down(int a_semid, int a_semnum)
{
    do_sem_operation(a_semid, a_semnum, -1);
}

void do_sem_operation(int a_semid, int a_semnum, int a_sem_op)
{
    buf.sem_num = a_semnum;
    buf.sem_op = a_sem_op;
    buf.sem_flg = 0;
    semop(a_semid, &buf, 1);
}