#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "fs.h"

/***************************************************
* vdisk		disc file pointer
* info		disc information structure
* cat		catalogue entry structure
* n		number structure for reading and writing
* faddr, addr	file or block addresses
* fblock	file block
* fblocks	how many blocks the file occupies
* prevaddr, i, flag	helping variables
******************************************************/
int fremove(char *name) {
  FILE *vdisk;
  struct d_info info;
  struct catalogue cat;
  struct number n;
  int faddr=0, addr, i, fblock, fblocks, prevaddr, flag=0;
  
  printf("removing file %s\n", name);
  vdisk = fopen("dysk", "r+b");
  if(vdisk == NULL) {
    printf("Error: cannot open disc file\n");
    return -1;
  }
  if(fseek(vdisk, 0, SEEK_SET) != 0) {
    printf("Error: fseek failed\n");
    return -1;
  }
  if(fread(&info, sizeof(info), 1, vdisk) != 1) {
    printf("Error: fread failed\n");
    return -1;
  }
  prevaddr = addr = info.first;
  if(fseek(vdisk, addr, SEEK_SET) != 0) {
    printf("Error: fseek failed\n");
    return -1;
  }
  i = 1;
  while(i<=info.fcount) {
    if(fread(&cat, sizeof(cat), 1, vdisk) != 1) {
      printf("Error: fread failed\n");
      return -1;
    }
    if(!strcmp(cat.name, name)) {
      faddr = (i-1)%(BLOCK/sizeof(cat));
      faddr *= sizeof(cat);
      faddr += addr;
      fblock = cat.addr / BLOCK;
      fblocks = cat.size / BLOCK + 1;
    }
    if(i % (BLOCK / sizeof(cat)) == 0) {
      prevaddr = addr;
      addr = blockVal(addr/BLOCK, vdisk);
      if(addr == -1) {
        return -1;
      }
      if(fseek(vdisk, addr, SEEK_SET) != 0) {
        printf("Error: fseek failed\n");
        return -1;
      }
    }
    ++i;
  }
  if(faddr == 0) {
    printf("Error: no file to remove\n");
    return -1;
  }
  if(fseek(vdisk, faddr, SEEK_SET) != 0) {
    printf("Error: fseek failed\n");
    return -1;
  }
  if(fwrite(&cat, sizeof(cat), 1, vdisk) != 1) {
    printf("Error: fwrite failed\n");
    return -1;
  }
  /*we might need to release a block from catalogue*/
  if(info.fcount % (BLOCK/sizeof(cat)) == 1) {
    /*addr - address of block to release
     *prevaddr = address of previous block in catalogue */
    if(fseek(vdisk, BLOCK + sizeof(n) * (addr/BLOCK), SEEK_SET) != 0) {
      printf("Error: fseek failed\n");
      return -1;
    }
    n.num = -1;
    if(fwrite(&n, sizeof(n), 1, vdisk) != 1) {
      printf("Error: fwrite failed\n");
      return -1;
    }
    if(fseek(vdisk, BLOCK + sizeof(n) * (prevaddr/BLOCK), SEEK_SET) != 0) {
      printf("Error: fseek failed\n");
      return -1;
    }
    n.num = 0;
    if(fwrite(&n, sizeof(n), 1, vdisk) != 1) {
      printf("Error: fwrite failed\n");
      return -1;
    }
    flag = 1;
  }

  info.fcount -= 1;
  info.free += (fblocks+flag)*BLOCK;
  if(fseek(vdisk, 0, SEEK_SET) != 0) {
    printf("Error: fseek failed\n");
    return -1;
  }
  if(fwrite(&info, sizeof(info), 1, vdisk) != 1) {
    printf("Error: fwrite failed\n");
    return -1;
  }

  while(addr != 0) {
    addr = blockVal(fblock, vdisk);
    if(addr == -2) {
      return -1;
    }
    if(fseek(vdisk, BLOCK + fblock*sizeof(n), SEEK_SET) != 0) {
      printf("Error: fseek failed\n");
      return -1;
    }
    n.num = -1;
    if(fwrite(&n, sizeof(n), 1, vdisk) != 1) {
      printf("Error: fseek failed\n");
      return -1;
    }
    fblock = addr / BLOCK;
  }
  if(fclose(vdisk) != 0) {
    printf("Error: fclose failed\n");
    return -1;
  }
  return 0;
}

