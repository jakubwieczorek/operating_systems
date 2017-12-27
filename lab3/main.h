#ifndef LAB3_MAIN_H
#define LAB3_MAIN_H

#include "semaphores.h"

void init(int argc, char *argv[]);
int build_sem(int a_key, int a_size);
void init_factory(factory** a_factory, char a_type, int a_robots_num, char *a_path);
int build_shm_id(int a_key, size_t a_size);
void create_robots(factory **a_factory);

#endif
