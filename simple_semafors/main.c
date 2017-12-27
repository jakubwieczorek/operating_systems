#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
int M,N,P,R,S,T;
key_t k1;
key_t k2;
key_t k3;
int key;
int semaphores;
enum {SEM_WRITE_Y, SEM_READ_Y, SEM_WRITE_Z, SEM_READ_Z, SEM_FREE_SPACE_COUNTER_Y,   SEM_BUSY_SPACE_COUNTER_Y, SEM_FREE_SPACE_COUNTER_Z, SEM_BUSY_SPACE_COUNTER_Z};
enum {IDX_WRITE_PLACE = 5, IDX_READ_PLACE, COUNTER_PLACE, COUNTER_BOTH};
const int queue_size = 5;
int shm_id_y, shm_id_z;
int *BY, *BZ;
int y_elem = 4;
int z_elem = 6;
int pid;
static struct sembuf sem_buf;

int licznikKonca;

int *procesy;

int semaforKoniec;

void up(int a_sem, int a_sem_num);
void down(int a_sem_id, int a_sem_num);
void build_elem_y();
void build_elem_z();
void build_yz();

int main(int argc, char *argv[])
{
    int i;

    if(argc != 7)
    {
        printf("liczba argumentow 6, kolejno MNPRST\n");
        return 1;
    }

    srand((unsigned int) time(NULL));

    M=atoi(argv[1]);
    N=atoi(argv[2]);
    P=atoi(argv[3]);
    R=atoi(argv[4]);
    S=atoi(argv[5]);
    T=atoi(argv[6]);

    // Utworzenie semaforów
    k1 = ftok(".", 'A');
    semaphores = semget(k1, 8, IPC_CREAT | 0600);
    k2 = ftok(".", 'B');
    semaforKoniec = semget(k2,1, IPC_CREAT|0600);

    // Nadanie wartosci poczatkowej
    semctl(semaphores, SEM_WRITE_Y, SETVAL, 1);
    semctl(semaphores, SEM_READ_Y, SETVAL, 1);
    semctl(semaphores, SEM_WRITE_Z, SETVAL, 1);
    semctl(semaphores, SEM_READ_Z, SETVAL, 1);
    semctl(semaphores, SEM_FREE_SPACE_COUNTER_Y, SETVAL, queue_size);
    semctl(semaphores, SEM_BUSY_SPACE_COUNTER_Y, SETVAL, 0);
    semctl(semaphores, SEM_FREE_SPACE_COUNTER_Z, SETVAL, queue_size);
    semctl(semaphores, SEM_BUSY_SPACE_COUNTER_Z, SETVAL, 0);

    // Przydzielenie pamieci wspoldzielonej
    shm_id_y = shmget(k1, (queue_size + 4) * sizeof(int), IPC_CREAT | 0600); // TODO zmien komentarze BY,indeksy do zapisu i odczytu BY, liczniki elementow;
    shm_id_z = shmget(k2, (queue_size + 4) * sizeof(int), IPC_CREAT | 0600); // TODO zmien na 3


    for(i = 0; i < N ; i++)
    {
        pid = fork();
        if(pid == 0)
        {
            // Podlaczenie pamieci wspoldzielonej
            BY = (int*)shmat(shm_id_y, NULL, 0);

            if(i == 0)
            {
                BY[COUNTER_PLACE] = 0;
                BY[IDX_READ_PLACE] = BY[IDX_WRITE_PLACE] = 0;
            }

            // Uruchomienie funkcji robotN()
            build_elem_y();
        }
    }

    for(i = 0; i < M ; i++)
    {
        if(fork() == 0)
        {
            // Podlaczenie pamieci wspoldzielonej
            BZ = (int*)shmat(shm_id_z, NULL, 0);

            if(i == 0)
            {
                BZ[COUNTER_PLACE] = 0;
                BZ[IDX_READ_PLACE] = BZ[IDX_WRITE_PLACE] = 0;
            }

            // Uruchomienie funkcji robotN()
            build_elem_z();
        }
    }

    for(i = 0; i < P ; i++)
    {
        // Utworzenie nowego procesu
        if(fork() == 0)
        {
            // Podlaczenie pamieci wspoldzielonej
            BY = (int*)shmat(shm_id_y, NULL, 0);
            BZ = (int*)shmat(shm_id_z, NULL, 0);

            build_yz();
        }
    }

    return EXIT_SUCCESS;
}

void build_yz()
{
    int y, z;
    while(1)
    {
        // y elem
        down(semaphores, SEM_BUSY_SPACE_COUNTER_Y);
        down(semaphores, SEM_READ_Y);

        if(BY[COUNTER_BOTH] < 10)
        {
            y = BY[IDX_READ_PLACE];
            BY[IDX_READ_PLACE] = (BY[IDX_READ_PLACE]+1) % BY[IDX_READ_PLACE];
            BY[COUNTER_BOTH]++;
            up(semaphores, SEM_FREE_SPACE_COUNTER_Y);
        }
        up(semaphores, SEM_READ_Y);

        // z
        down(semaphores, SEM_BUSY_SPACE_COUNTER_Z);
        down(semaphores, SEM_READ_Z);

        if(BZ[COUNTER_BOTH] < 10)
        {
            z = BZ[IDX_READ_PLACE];
            BZ[IDX_READ_PLACE] = (BZ[IDX_READ_PLACE]+1) % BZ[IDX_READ_PLACE];
            BZ[COUNTER_BOTH]++;
            sleep((unsigned int) (rand() % T + 1));
            printf("linking two elems %d%d\n", y, z);
            up(semaphores, SEM_FREE_SPACE_COUNTER_Z);
        }

        up(semaphores, SEM_READ_Z);

        if(BZ[COUNTER_BOTH] == 10)
        {
            break;
        }
    }
}

void build_elem_y()
{
    while(1)
    {
        down(semaphores, SEM_FREE_SPACE_COUNTER_Y);
        down(semaphores, SEM_WRITE_Y); // lock

        if(BY[COUNTER_PLACE] < 10)
        { // BY [0, 1, 2, 3, 4, piszemyY, czytamyY, licznikY

            sleep((unsigned int) (rand() % R + 1));
            // IDX_WRITE_PLACE == 5 BY[IDX_WRITE_PLACE] jest miejsce do pisania np 1
            BY[BY[IDX_WRITE_PLACE]] = y_elem; // w miejsce do pisania wpisz wartosc
            BY[IDX_WRITE_PLACE]=(BY[IDX_WRITE_PLACE] + 1) % queue_size; // przesun indeks zapisu
            // z powrotem na poczatek gdy dojdzie do konca kolejki
            BY[COUNTER_PLACE]++;
            // printf("build elem y = %d", y_elem);
            up(semaphores, SEM_WRITE_Y);
        }
        up(semaphores, SEM_BUSY_SPACE_COUNTER_Y); // unlock

        if(BY[COUNTER_PLACE] == 10)
        {
            break;
        }
    }
}

void up(int a_sem_id, int a_sem_num)
{
    sem_buf.sem_flg = 0;
    sem_buf.sem_num = a_sem_num;
    sem_buf.sem_op = 1;

    semop(a_sem_id, &sem_buf, 1);
}

void down(int a_sem_id, int a_sem_num)
{
    sem_buf.sem_flg = 0;
    sem_buf.sem_num = a_sem_num;
    sem_buf.sem_op = -1;

    semop(a_sem_id, &sem_buf, 1);
}

void build_elem_z()
{
    while(1)
    {
        down(semaphores, SEM_FREE_SPACE_COUNTER_Z);
        down(semaphores, SEM_WRITE_Z); // lock

        if(BZ[COUNTER_PLACE] < 10)
        { // BY [0, 1, 2, 3, 4, piszemyY, czytamyY, licznikY

            sleep((unsigned int) (rand() % S + 1));
            // IDX_WRITE_PLACE == 5 BY[IDX_WRITE_PLACE] jest miejsce do pisania np 1
            BZ[BZ[IDX_WRITE_PLACE]] = z_elem; // w miejsce do pisania wpisz wartosc
            BZ[IDX_WRITE_PLACE]=(BZ[IDX_WRITE_PLACE] + 1) % queue_size; // przesun indeks zapisu
            // z powrotem na poczatek gdy dojdzie do konca kolejki
            BZ[COUNTER_PLACE]++;
            //printf("build elem z = %d", z_elem);
            up(semaphores, SEM_WRITE_Z);
        }
        up(semaphores, SEM_BUSY_SPACE_COUNTER_Z); // unlock

        if(BZ[COUNTER_PLACE] == 10)
        {
            break;
        }
    }
}