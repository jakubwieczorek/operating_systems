#include <stdio.h>

/********************************
* size of one block
********************************/
#define BLOCK 2048

/****************************************************
* size - the size of virtual disk
* fcount - number of files on virtual disk
* first - address of the first block of catalogue
* free - amount of free space on disk
****************************************************/
struct d_info {
  int size;
  int fcount;
  int first;
  int free;
};

/****************************************************
num - number to write to file
*****************************************************/
struct number {
  int num;
};

/*****************************************************
* name - name of the file
* size - file size in bytes
* addr - address of the first block of file
*****************************************************/
struct catalogue {
  char name[10];
  int size;
  int addr;
};

/***********************************************
* creates virtual disk.
* parameter: size of the disk
************************************************/
int vdiskCreate(int);

/********************************************************
* return the value for block:
* -1	the block is free for writing
* 0	the block is valid, and is the last of the file
* >0	the block is valid, and the value is the address of next block
*parameters: number of block, and name of the disk file
**********************************************************/
int blockVal(int, FILE*);

/**********************************************************
* copies a file from minix disk to virtual disk
* parameter: name of file to copy
***********************************************************/
int m2v(char*);

/***********************************************************
* copies a file from virtual disk to minix disk
* paramter: name of file to copy
***********************************************************/
int v2m(char*);

/**********************************************************
* lists all files on virtual disk
**********************************************************/
int ls(void);

/**********************************************************
* prints disk summary
**********************************************************/
int disc_stat(void);

/**********************************************************
* deletes the virtual disc
**********************************************************/
int del_disc(void);

/**********************************************************
* remove file from virtual disc
* paramter: file name
**********************************************************/
int fremove(char *);

