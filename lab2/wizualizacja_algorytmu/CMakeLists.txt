cmake_minimum_required(VERSION 3.7)
project(sched)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c proc.c proc.h const.h)
add_executable(sched ${SOURCE_FILES} proc.c proc.h)