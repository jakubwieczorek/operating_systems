#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define ROZMIAR_BUFORA 5


int M,N,P,R,S,T;
int semafory,semaforKoniec;
int *BY,*BZ;
int shmidY,shmidZ;
key_t k1,k2,k3; // klucze do tworzenia semaforow
int pid;
int *procesy;
int licznikKonca;
static struct sembuf buf;

void robotN();
void robotM();
void robotP();
void podnies(int semid, int semnum);
void opusc(int semid, int semnum);
void wartosci();

enum {SEM_WRITE_Y, SEM_READ_Y, SEM_WRITE_Z, SEM_READ_Z, SEM_FREE_SPACE_COUNTER_Y, SEM_BUSY_SPACE_COUNTER_Y, SEM_FREE_SPACE_COUNTER_Z, SEM_BUSY_SPACE_COUNTER_Z};
enum {IDX_WRITE_PLACE = 5, IDX_READ_PLACE, COUNTER_PLACE, COUNTER_BOTH};

int main(int argc, char** argv)
{
    int i;
    if(argc != 7)
    {
        printf("Nieprawidlowa liczba argumentow!\nSprobuj ponownie podajac wartosc M N P R S T\n");
        return 1;
    }

    M=atoi(argv[1]);
    N=atoi(argv[2]);
    P=atoi(argv[3]);
    R=atoi(argv[4]);
    S=atoi(argv[5]);
    T=atoi(argv[6]);

    srand(time(0));

    // Utworzenie semaforów
    k1=ftok(".",'A');
    semafory = semget(k1,8, IPC_CREAT|0600);
    k2=ftok(".",'B');
    semaforKoniec = semget(k2,1, IPC_CREAT|0600);

    // Nadanie wartosci poczatkowej
    semctl(semafory,0,SETVAL,1);
    semctl(semafory,1,SETVAL,1);
    semctl(semafory,2,SETVAL,1);
    semctl(semafory,3,SETVAL,1);
    semctl(semafory,4,SETVAL,(int)ROZMIAR_BUFORA);
    semctl(semafory,5,SETVAL,0);
    semctl(semafory,6,SETVAL,(int)ROZMIAR_BUFORA);
    semctl(semafory,7,SETVAL,0);

    //semctl(semaforKoniec,0,SETVAL,1);

    // Przydzielenie pamieci wspoldzielonej
    shmidY=shmget(k1, (ROZMIAR_BUFORA+4)*sizeof(int),IPC_CREAT|0600); // BY,indeksy do zapisu i odczytu BY, liczniki elementow;
    shmidZ=shmget(k2, (ROZMIAR_BUFORA+4)*sizeof(int),IPC_CREAT|0600);
    //k3=ftok(".",'C');
//    licznikKonca=shmget(k3,sizeof(int),IPC_CREAT|0600);
//    procesy=(int*)shmat(licznikKonca,NULL,0);
//#define licznikProcesow procesy[0]
//    licznikProcesow=N+M+P;

    // Tworzenie robotow:
    for(i=0;i<N;i++)
    {
        // Utworzenie nowego procesu
        pid=fork();
        if(pid==0)
        {
            printf("HELLO2");
            // Podlaczenie pamieci wspoldzielonej
            BY=(int*)shmat(shmidY,NULL,0);
            if(i==0)
            {
                BY[ROZMIAR_BUFORA+2]=BY[ROZMIAR_BUFORA+3]=0;
                BY[ROZMIAR_BUFORA]=BY[ROZMIAR_BUFORA+1]=0;
            }

            // Uruchomienie funkcji robotN()
            robotN();
        }
    }
    for(i=0;i<M;i++)
    {
        // Utworzenie nowego procesu
        pid=fork();
        if(pid==0)
        {
            // Podlaczenie pamieci wspoldzielonej
            BZ=(int*)shmat(shmidZ,NULL,0);
            if(i==0)
            {
                BZ[ROZMIAR_BUFORA+2]=BZ[ROZMIAR_BUFORA+3]=0;
                BZ[ROZMIAR_BUFORA]=BZ[ROZMIAR_BUFORA+1]=0;
            }

            // Uruchomienie funkcji robota
            robotM();
        }
    }
    for(i=0;i<P;i++)
    {
        // Utworzenie nowego procesu
        pid=fork();
        if(pid==0)
        {
            // Podlaczenie pamieci wspoldzielonej
            BY=(int*)shmat(shmidY,NULL,0);
            BZ=(int*)shmat(shmidZ,NULL,0);

            robotP();
        }
    }

    return 0;
}



void robotN()
{
    while(1)
    {
        opusc(semafory,SEM_FREE_SPACE_COUNTER_Y);
        opusc(semafory,SEM_WRITE_Y);
        printf("HELLO");
        if(BY[ROZMIAR_BUFORA+2]<10)
        {
            sleep(rand()%R+1);
            BY[BY[ROZMIAR_BUFORA]]=BY[ROZMIAR_BUFORA+2];
            BY[ROZMIAR_BUFORA]=(BY[ROZMIAR_BUFORA]+1)%ROZMIAR_BUFORA;
            BY[ROZMIAR_BUFORA+2]++;
            podnies(semafory,SEM_BUSY_SPACE_COUNTER_Y);
        }
        podnies(semafory,SEM_WRITE_Y);

        if(BY[ROZMIAR_BUFORA+2]==10)
            break;
    }
//    // ,,Odsmiecanie"
//    opusc(semaforKoniec,0);
//    licznikProcesow--;
//    shmdt((char*)BY);
//    if(licznikProcesow==0)
//    {
//        shmdt((char *)licznikProcesow);
//        semctl(semaforKoniec,0,IPC_RMID);
//        exit(0) ;
//    }
//    podnies(semaforKoniec,0);
//    exit(0);
}

void robotM()
{
    while(1)
    {
        opusc(semafory,SEM_FREE_SPACE_COUNTER_Z);
        opusc(semafory,SEM_WRITE_Z);
        if(BZ[ROZMIAR_BUFORA+2]<10)
        {
            sleep(rand()%S+1);
            BZ[BZ[ROZMIAR_BUFORA]]=BZ[ROZMIAR_BUFORA+2];
            BZ[ROZMIAR_BUFORA]=(BZ[ROZMIAR_BUFORA]+1)%ROZMIAR_BUFORA;
            BZ[ROZMIAR_BUFORA+2]++;
            podnies(semafory,SEM_BUSY_SPACE_COUNTER_Z);
        }
        podnies(semafory,SEM_WRITE_Z);
        if(BZ[ROZMIAR_BUFORA+2]==10)
            break;
    }
//    opusc(semaforKoniec,0);
//    licznikProcesow--;
//    shmdt((char*)BZ);
//    if(licznikProcesow==0)
//    {
//        shmdt((char *)licznikProcesow);
//        semctl(semaforKoniec,0,IPC_RMID);
//        exit(0);
//    }
//    podnies(semaforKoniec,0);
//    exit(0);
}

void robotP()
{
    int pierwszyElement,drugiElement;
    while(1)
    {
        opusc(semafory,SEM_BUSY_SPACE_COUNTER_Y);
        opusc(semafory,SEM_READ_Y);
        if(BY[ROZMIAR_BUFORA+3]<10)
        {
            pierwszyElement=BY[BY[ROZMIAR_BUFORA+1]];
            BY[ROZMIAR_BUFORA+1]=(BY[ROZMIAR_BUFORA+1]+1)%ROZMIAR_BUFORA;
            BY[ROZMIAR_BUFORA+3]++;
            podnies(semafory,SEM_FREE_SPACE_COUNTER_Y);
        }
        podnies(semafory,SEM_READ_Y);

        opusc(semafory,SEM_BUSY_SPACE_COUNTER_Z);
        opusc(semafory,SEM_READ_Z);
        if(BZ[ROZMIAR_BUFORA+3]<10)
        {
            drugiElement=BZ[BZ[ROZMIAR_BUFORA+1]];
            BZ[ROZMIAR_BUFORA+1]=(BZ[ROZMIAR_BUFORA+1]+1)%ROZMIAR_BUFORA;
            BZ[ROZMIAR_BUFORA+3]++;
            podnies(semafory,SEM_FREE_SPACE_COUNTER_Z);
            sleep(rand()%T+1);
            printf("Wyprodukowano element %d%d\n",pierwszyElement,drugiElement);
        }
        podnies(semafory,SEM_READ_Z);
        if(BZ[ROZMIAR_BUFORA+3] == 10)
            break;
    }
//    opusc(semaforKoniec,0);
//    //--licznikProcesow;
//    shmctl(shmidY, IPC_RMID, (struct shmid_ds*)0); // usuniecie pamieci wspoldzielonej
//    shmctl(shmidZ, IPC_RMID, (struct shmid_ds*)0);
//    shmctl(licznikKonca, IPC_RMID, (struct shmid_ds* )0);
//
//    // shmdt(char* shmaddr);
//    shmdt((char*)BY);
//    shmdt((char*)BZ);
//
//    semctl(semafory,0,IPC_RMID);
//    if(licznikProcesow==0)
//    {
//        shmdt((char *)licznikProcesow);
//        semctl(semaforKoniec,0,IPC_RMID);
//        exit(0);
//    }
//    podnies(semaforKoniec,0);
//    exit(0);
}

void podnies(int semid, int semnum)
{
    buf.sem_num = semnum;
    buf.sem_op = 1;
    buf.sem_flg = 0;
    semop(semid,&buf,1);
}

void opusc(int semid, int semnum)
{
    buf.sem_num = semnum;
    buf.sem_op = -1;
    buf.sem_flg = 0;
    semop(semid,&buf,1);
}
void wartosci(){
    unsigned short wartosci[8];
    semctl(semafory,0,GETALL,(unsigned short*)wartosci);
    printf("Semafor SEM_WRITE_Y = %d\n",wartosci[0]);
    printf("Semafor SEM_READ_Y = %d\n",wartosci[1]);
    printf("Semafor SEM_WRITE_Z = %d\n",wartosci[2]);
    printf("Semafor SEM_READ_Z = %d\n",wartosci[3]);
    printf("Semafor SEM_FREE_SPACE_COUNTER_Y = %d\n",wartosci[4]);
    printf("Semafor SEM_BUSY_SPACE_COUNTER_Y = %d\n",wartosci[5]);
    printf("Semafor SEM_FREE_SPACE_COUNTER_Z = %d\n",wartosci[6]);
    printf("Semafor SEM_BUSY_SPACE_COUNTER_Z = %d\n",wartosci[7]);

}