#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int main()
{
    int shm;

    if((shm = shmget(1111, sizeof(queue), 0666 | IPC_CREAT)) == -1)
    {
        perror("shmget error");
        exit(-1);
    }

    void* mem_seg;

    if((mem_seg = shmat(shm, NULL, 0)) == (void*) -1)
    {
        perror("shmat error");
        exit(-1);
    }

    if(shmdt(mem_seg) == -1)
    {
        perror("shmdt error");
        exit(-1);
    }
}
