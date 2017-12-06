cmake_minimum_required(VERSION 3.7)
project(lab3)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES example.c)
add_executable(example example.c)
add_executable(main main.c main.h semaphores.c semaphores.h queue.c queue.h)