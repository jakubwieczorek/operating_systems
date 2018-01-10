#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "fs.h"

/*********************************************************
* vdisk		disc file pointer
* blocks	number of blocks on disc
* sblocks	number of special blocks on disc
* buffer	block-sized buffer for data writing
* info		disc information structure
* n		number structure for disc writing
* i		iteration variable
*********************************************************/
int vdiskCreate(int size) {
  FILE *vdisk;
  int blocks, sblocks;
  int i;
  char buffer[BLOCK];
  struct d_info info;
  struct number n;

  size *= 1024;
  blocks = size / BLOCK;
  
  vdisk = fopen("dysk", "wb+");
  if(vdisk == NULL) {
    printf("Error: cannot create disc\n");
    return -1;
  }
  if(fseek(vdisk, size-1, SEEK_SET) != 0) {
    printf("Error: cannot set disc size (fseek)\n");
    return -1;
  }
  buffer[0] = 'a';
  if(fwrite(buffer, sizeof(char), 1, vdisk) != 1) {
    printf("Error: cannot set disc size (fwrite)\n");
    return -1;
  }

  /*writing statistics block*/
  if(fseek(vdisk, 0, SEEK_SET) != 0)  {
    printf("Error: cannot rewind disc\n");
    return -1;
  }
  info.size = size;
  info.fcount = 0;
  info.free = size;
  sblocks = (sizeof(int)*blocks);
  sblocks = sblocks / BLOCK + 1;
  info.first = (sblocks+1) * BLOCK;
  if(fwrite(&info, sizeof(info), 1, vdisk) != 1) {
    printf("Error: cannot write data\n");
    return -1;
  }

  /*starting to write content table*/
  if(fseek(vdisk, BLOCK, SEEK_SET) != 0) {
    printf("Error: fseek failed\n");
    return -1;
  }
  blocks = size / BLOCK; /*blocks in vdisk*/
  n.num = 0;
  if(fwrite(&n, sizeof(n), 1, vdisk) != 1) {
    printf("Error: cannot write data\n");
    return -1;
  }
  for(i = 0; i < sblocks; ++i) {
    if(fwrite(&n, sizeof(n), 1, vdisk) != 1) {
      printf("Error: cannot write data\n");
      return -1;
    }
  }
  n.num = -1;
  for(i = 0; i < blocks-sblocks; ++i) {
    if(fwrite(&n, sizeof(n), 1, vdisk) != 1) {
      printf("Error: cannot write data\n");
      return -1;
    }
  }

  if(fclose(vdisk) != 0) {
    printf("Error: cannot close disc file\n");
    return -1;
  }
  return 0;
}

