#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define BUFFER_SIZE 5

int P, T;
int semaforKoniec;

enum
{
    ID_WRITE_MTX, ID_READ_MTX,
    FREE_COUNTER, BUSY_COUNTER
};

struct factory
{
    int semaphores;
    key_t sem_mem_key;
    int *buffer;
    int mem_id;
    int robots_number;
    int max_work_time;
}y_factory, z_factory;

key_t k3, k4; // klucze do tworzenia semaforow
int *procesy;
int licznikKonca;
static struct sembuf buf;

void create_elements(struct factory a_factory);
void create_robots(struct factory a_factory);
void init_factory(struct factory a_factory);
void clear_factory(struct factory a_factory);
void unlock(int semid, int semnum);
void lock(int semid, int semnum);
void robotP();

// void create_robot(int a_size);

int main(int argc, char** argv)
{
    if(argc != 7)
    {
        printf("Nieprawidlowa liczba argumentow\nSprobuj ponownie podajac wartosc M N P R S T\n");
        return 1;
    }

    z_factory.robots_number=atoi(argv[1]);
    y_factory.robots_number=atoi(argv[2]);
    P=atoi(argv[3]);
    y_factory.max_work_time=atoi(argv[4]);
    z_factory.max_work_time=atoi(argv[5]);
    T=atoi(argv[6]);

    srand(time(0));

    // Utworzenie semafor�w
    y_factory.sem_mem_key=ftok(".",'A');
    k4=ftok(".",'D');
    y_factory.semaphores = semget(y_factory.sem_mem_key,4, IPC_CREAT|0600);
    z_factory.semaphores = semget(k4,4, IPC_CREAT|0600);
    z_factory.sem_mem_key=ftok(".",'B');
    semaforKoniec = semget(z_factory.sem_mem_key,1, IPC_CREAT|0600);

    init_factory(y_factory);
    init_factory(z_factory);

    semctl(semaforKoniec,0,SETVAL,1);

    // Przydzielenie pamieci wspoldzielonej
    y_factory.mem_id=shmget(y_factory.sem_mem_key, (BUFFER_SIZE+4)*sizeof(int),IPC_CREAT|0600); // y_factory.buffer,indeksy do zapisu i odczytu y_factory.buffer, liczniki elementow;
    z_factory.mem_id=shmget(z_factory.sem_mem_key, (BUFFER_SIZE+4)*sizeof(int),IPC_CREAT|0600);

    k3=ftok(".",'C');
    licznikKonca=shmget(k3,sizeof(int),IPC_CREAT|0600);
    procesy=(int*)shmat(licznikKonca,NULL,0);
    #define licznikProcesow procesy[0]
    licznikProcesow=y_factory.robots_number+z_factory.robots_number+P;

    // Tworzenie robotow:
    create_robots(y_factory);
    create_robots(z_factory);

    for(int i=0;i<P;i++)
    {
        // Utworzenie nowego procesu
        if(fork() == 0)
        {
            // Podlaczenie pamieci wspoldzielonej
            y_factory.buffer=(int*)shmat(y_factory.mem_id,NULL,0);
            z_factory.buffer=(int*)shmat(z_factory.mem_id,NULL,0);

            robotP();
        }
    }

    return 0;
}

void init_factory(struct factory a_factory)
{
    // Nadanie wartosci poczatkowej
    semctl(a_factory.semaphores, ID_WRITE_MTX, SETVAL, 1);
    semctl(a_factory.semaphores, ID_READ_MTX, SETVAL, 1);
    semctl(a_factory.semaphores, FREE_COUNTER, SETVAL, (int)BUFFER_SIZE);
    semctl(a_factory.semaphores, BUSY_COUNTER, SETVAL, 0);
}

void create_robots(struct factory a_factory)
{
    for(int i=0; i < a_factory.robots_number; i++)
    {
        // create new process and if child:
        if(fork() == 0)
        {
            // Podlaczenie pamieci wspoldzielonej
            a_factory.buffer = (int*) shmat(a_factory.mem_id, NULL, 0);

            if(i == 0)
            {
                a_factory.buffer[BUFFER_SIZE+2]=a_factory.buffer[BUFFER_SIZE+3]=0;
                a_factory.buffer[BUFFER_SIZE]=a_factory.buffer[BUFFER_SIZE+1]=0;
            }

            create_elements(a_factory);
        }
    }
}

void create_elements(struct factory a_factory)
{
    while(1)
    {
        lock(a_factory.semaphores, FREE_COUNTER);
        lock(a_factory.semaphores, ID_WRITE_MTX);

        if(a_factory.buffer[BUFFER_SIZE+2]<10)
        {
            sleep(rand()%y_factory.max_work_time+1);
            a_factory.buffer[a_factory.buffer[BUFFER_SIZE]]=a_factory.buffer[BUFFER_SIZE+2];
            a_factory.buffer[BUFFER_SIZE]=(a_factory.buffer[BUFFER_SIZE]+1)%BUFFER_SIZE;
            a_factory.buffer[BUFFER_SIZE+2]++;
            unlock(a_factory.semaphores,BUSY_COUNTER);
        }
        unlock(a_factory.semaphores,ID_WRITE_MTX);

        if(a_factory.buffer[BUFFER_SIZE+2]==10)
            break;
    }

    // ,,Odsmiecanie"
    clear_factory(a_factory);

}

void clear_factory(struct factory a_factory)
{
    lock(semaforKoniec,0);
    licznikProcesow--;
    shmdt((char*)a_factory.buffer);
    if(licznikProcesow==0)
    {
        shmdt((char *)licznikProcesow);
        semctl(semaforKoniec,0,IPC_RMID);
        exit(0) ;
    }
    unlock(semaforKoniec,0);
    exit(0);
}

void robotP()
{
    int pierwszyElement,drugiElement;
    while(1)
    {
        lock(y_factory.semaphores,BUSY_COUNTER);
        lock(y_factory.semaphores,ID_READ_MTX);
        if(y_factory.buffer[BUFFER_SIZE+3]<10)
        {
            pierwszyElement=y_factory.buffer[y_factory.buffer[BUFFER_SIZE+1]];
            y_factory.buffer[BUFFER_SIZE+1]=(y_factory.buffer[BUFFER_SIZE+1]+1)%BUFFER_SIZE;
            y_factory.buffer[BUFFER_SIZE+3]++;
            unlock(y_factory.semaphores,FREE_COUNTER);
        }
        unlock(y_factory.semaphores,ID_READ_MTX);

        lock(z_factory.semaphores,BUSY_COUNTER);
        lock(z_factory.semaphores,ID_READ_MTX);
        if(z_factory.buffer[BUFFER_SIZE+3]<10)
        {
            drugiElement=z_factory.buffer[z_factory.buffer[BUFFER_SIZE+1]];
            z_factory.buffer[BUFFER_SIZE+1]=(z_factory.buffer[BUFFER_SIZE+1]+1)%BUFFER_SIZE;
            z_factory.buffer[BUFFER_SIZE+3]++;
            unlock(z_factory.semaphores,FREE_COUNTER);
            sleep(rand()%T+1);
            printf("Wyprodukowano element %d%d\n",pierwszyElement,drugiElement);
        }
        unlock(z_factory.semaphores,ID_READ_MTX);
        if(z_factory.buffer[BUFFER_SIZE+3] == 10)
            break;
    }
    lock(semaforKoniec,0);
    --licznikProcesow;
    shmctl(y_factory.mem_id, IPC_RMID, (struct shmid_ds*)0); // usuniecie pamieci wspoldzielonej
    shmctl(z_factory.mem_id, IPC_RMID, (struct shmid_ds*)0);
    shmctl(licznikKonca, IPC_RMID, (struct shmid_ds* )0);
   
   // shmdt(char* shmaddr);
    shmdt((char*)y_factory.buffer); 
    shmdt((char*)z_factory.buffer);
	
    semctl(y_factory.semaphores,0,IPC_RMID);
    semctl(z_factory.semaphores,0,IPC_RMID);
    if(licznikProcesow==0)
    {
    	shmdt((char *)licznikProcesow);
        semctl(semaforKoniec,0,IPC_RMID);
	    exit(0);
    }
    unlock(semaforKoniec,0);
    exit(0);
}

void unlock(int semid, int semnum)
{
    buf.sem_num = semnum;
    buf.sem_op = 1;
    buf.sem_flg = 0;
    semop(semid,&buf,1);
}

void lock(int semid, int semnum)
{
    buf.sem_num = semnum;
    buf.sem_op = -1;
    buf.sem_flg = 0;
    semop(semid,&buf,1);
}