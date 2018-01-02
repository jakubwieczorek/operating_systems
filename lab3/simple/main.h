#ifndef main_h
#define main_h

#define BUFFER_SIZE 5

enum
{
    WRITE_MTX, READ_MTX,
    FREE_COUNTER, BUSY_COUNTER,
};

enum
{
    BUF_IDX_WRITE = BUFFER_SIZE, BUF_IDX_READ,
    BUF_BUSY_COUNTER, BUF_FREE_COUNTER
};

struct producer_factory // producer
{
    int semaphores;
    key_t sem_mem_key;
    int *buffer;
    struct queue *q_buffer;
    int mem_id;
    int robots_number;
    int max_work_time;
}y_factory, z_factory;


int P, T;
int end_protector_sem;

// klucze do tworzenia semaforow i pamieci wspodzielonej
key_t k3, k4;
int *process_counter;
int end_counter_shm;
static struct sembuf buf;

void create_elements(struct producer_factory* a_factory);
void create_robots(struct producer_factory* a_factory);
void init_factory(struct producer_factory* a_factory);
void clear_factory(struct producer_factory* a_factory);
void do_sem_operation(int a_semid, int a_semnum, int a_sem_op);
void unlock(int semid, int semnum);
void lock(int semid, int semnum);
void up(int a_semid, int a_semnum);
void down(int a_semid, int a_semnum);
int consume_element(struct producer_factory* a_factory);
void consume_elements();
void delete_shm_sem(struct producer_factory* a_factory);

#endif
