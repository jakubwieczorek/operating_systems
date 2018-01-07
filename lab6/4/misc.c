#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "fs.h"

/********************************************
* vdisk		disc file pointer
* cat		catalogue entry structure
* indo		disc info structure
* addr		block address
* i		iteration variable
********************************************/
int ls(void) {
  FILE *vdisk;
  struct catalogue cat;
  struct d_info info;
  int i, addr;

  vdisk = fopen("dysk", "r");
  if(vdisk==NULL) {
    printf("Error: no virtual disc\n");
    return -1;
  }
  if(fread(&info, sizeof(info), 1, vdisk) != 1) {
    printf("Error: fread failed\n");
    return -1;
  }
  printf("Total: %d files\n\n", info.fcount);
  addr = info.first;
  if(fseek(vdisk, addr, SEEK_SET) != 0) {
    printf("Error: fseek failed\n");
    return -1;
  }
  printf("Name\tsize\taddress:\n\n");
  for(i=1; i<=info.fcount; ++i) {
    if(fread(&cat, sizeof(cat), 1, vdisk) != 1) {
      printf("Error: fread failed\n");
      return -1;
    }
    printf("%s\t%d\t%d\n", cat.name, cat.size, cat.addr);
    if(i % (BLOCK/sizeof(cat)) == 0) {
      addr = blockVal(addr/BLOCK, vdisk);
      if(addr == -2) {
        return -1;
      }
      if(fseek(vdisk, addr, SEEK_SET) != 0) {
        printf("Error: fread failed\n");
        return -1;
      }
    }
  }
  printf("done\n");
  if(fclose(vdisk) != 0) {
    printf("Error: fclose failed\n");
    return -1;
  }
  return 0;
}

int disc_stat(void) {
  FILE *vdisk;
  struct d_info info;
  struct number n;
  int i, blocks, valid=0, invalid=0;
  
  vdisk = fopen("dysk", "r");
  if(vdisk==NULL) {
    printf("Error: no virtual disc\n");
    return -1;
  }
  if(fread(&info, sizeof(info), 1, vdisk) != 1) {
    printf("Error: fread failed\n");
    return -1;
  }
  printf("Disk size: %d KB\n", info.size/BLOCK);
  printf("Free space: %d KB\n", info.free/BLOCK);
  printf("Number of files: %d\n", info.fcount);
  
  blocks = info.size/BLOCK;
  printf("Total blocks: %d, ", blocks);
  i = 0;
  while(i < blocks) {
    if(fread(&n, sizeof(n), 1, vdisk) != 1) {
      printf("Error: fread failed\n");
      return -1;
    }
    if(n.num >= 0) ++valid;
    else ++invalid;
    ++i;
  }
  printf("free %d, valid %d\n", invalid, valid);
  if(fclose(vdisk) != 0) {
    printf("Error: fclose failed\n");
    return -1;
  }
  return 0;
}

int del_disc(void) {
  return unlink("dysk");
}
