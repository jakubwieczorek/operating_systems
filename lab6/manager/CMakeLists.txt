cmake_minimum_required(VERSION 3.7)
project(manager)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp main.h MemoryManager.cpp MemoryManager.h VirtualDisc.cpp VirtualDisc.h Validator.cpp Validator.h VirtualFile.cpp VirtualFile.h VirtualHole.cpp VirtualHole.h)
add_executable(manager ${SOURCE_FILES} main.h MemoryManager.cpp MemoryManager.h VirtualDisc.cpp VirtualDisc.h Validator.cpp Validator.h VirtualFile.cpp VirtualFile.h VirtualHole.cpp VirtualHole.h)