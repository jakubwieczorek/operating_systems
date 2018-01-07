#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "fs.h"

/**************************************************
* blocks	number of blocks on disc
* info		disc information structure
* n		number structure for disc reading
***************************************************/
int blockVal(int nr, FILE* vdisk) {
  int blocks;
  struct d_info info;
  struct number n;

  if(fseek(vdisk, 0, SEEK_SET) != 0) {
    printf("Error: cannot rewind disc file\n");
    return -2;
  }
  if(fread(&info, sizeof(info), 1, vdisk) != 1) {
    printf("Error: cannot read disc info\n");
    return -2;
  }
  blocks = info.size / BLOCK;
  if(nr < 0 || nr > blocks) {
    printf("Error (blockVal): wrong block nr\n");
    exit(1);
  }
  if(fseek(vdisk, BLOCK + nr*sizeof(n), SEEK_SET) != 0) {
    printf("Error: fseek failed\n");
    return -2;
  }
  if(fread(&n, sizeof(n), 1, vdisk) != 1) {
    printf("cannot read block value\n");
    return -1;
  }
  return n.num;
}

/*************************************************
* vdisk		disc file pointer
* fp		pointer to copied filed
* buffer	block-sized buffer for writing data
* info		disc info structure
* kat		one catalogue entry
* n		number structure for reading and writing
* fsize, size	file size
* addr, faddr	file or block address
* block, actBlock, prevBlock	in which block we are
* flag, i	additional varaibles
***************************************************/
int m2v(char *name) {
  FILE *vdisk, *fp;
  char buffer[BLOCK];
  struct d_info info;
  struct catalogue kat;
  struct number n;
  int i, size, addr, block = 0, actBlock, prevBlock, flag = 0;
  int fsize, faddr, j , kaddr;

  printf("copying %s to virtual disk\n", name);
  vdisk = fopen("dysk", "r+b");
  if(vdisk == NULL) {
    printf("Error: cannot open disc file\n");
    return -1;
  }
  fp = fopen(name, "r");
  if(fp == NULL) {
    printf("Error: no such file on minix disc\n");
    return -1;
  }
  if(fseek(fp, 0, SEEK_END) != 0) {
    printf("Error: cannot seek to the end of file\n");
    return -1;
  }
  size = fsize = ftell(fp);
  if(size == -1L) {
    printf("Error: ftell failed\n");
    return -1;
  }
  rewind(fp);
  rewind(vdisk);

  if(fread(&info, sizeof(info), 1, vdisk) != 1) {
    printf("Error: cannot read disc info\n");
    return -1;
  }
  if(fsize > info.free) {
    printf("Error: Insufficient free space\n");
    return -1;
  }
  i = strlen(name);
  if(i > 10 || i<=0) {
    printf("Error: file name too long or too short\n");
    return -1;
  }

  if(info.fcount == 0) {
    strcpy(kat.name, name);
    kat.size = size;
    kat.addr = info.first + BLOCK;
    if(fseek(vdisk, BLOCK + sizeof(n)*info.first/BLOCK, SEEK_SET) != 0) {
      printf("Error: fseek failed\n");
      return -1;
    }
    n.num = 0;
    if(fwrite(&n, sizeof(n), 1, vdisk) != 1) {
      printf("Error: fwrite 1 failed\n");
      return -1;
    }
    kaddr = info.first;
    flag = 1;
  }

  i = 0;
  /*find the address of first free block, for the file*/
  while(1) {
    block = blockVal(i, vdisk);
    if(block == -2) {
      return -1;
    }
    if(fseek(vdisk, 0, SEEK_SET) != 0) {
      printf("Error: fseek failed\n");
      return -1;
    }
    if(fread(&info, sizeof(info), 1, vdisk) != 1) {
      printf("Error: fread 1 failed\n");
      return -1;
    }
    if(block == -1) {
      addr = faddr = BLOCK*i;
      if(fseek(vdisk, BLOCK + i*sizeof(n), SEEK_SET) != 0) {
        printf("Error: fseek failed\n");
        return -1;
      }
      n.num = 0;
      if(fwrite(&n, sizeof(n), 1, vdisk) != 1) {
        printf("Error: fwrite 2 failed\n");
        return -1;
      }
      break;
    }
    ++i;
  }
  
  if(!flag) {
    /*searching through catalogue*/
    if(fseek(vdisk, info.first, SEEK_SET) != 0) {
      printf("Error: fseek failed\n");
      return -1;
    }
    addr = info.first;
    i = 1;
    actBlock = prevBlock = addr/BLOCK;
    while(i <= info.fcount) {
      if(fread(&kat, sizeof(kat), 1, vdisk) != 1) {
        printf("Error: fread 2 failed\n");
        return -1;
      }
      if(!strcmp(kat.name, name)) {
        printf("Error: File already exists\n");
        return -1;
      }
      addr += sizeof(kat);
      /*need to go to another block*/
      if(i % (BLOCK/sizeof(kat)) == 0) {
        /*we might get here, and finish the loop*/
        addr = blockVal(actBlock, vdisk);
        if(addr == -2) {
          return -1;
        }
        if(fseek(vdisk, addr, SEEK_SET) != 0) {
          printf("Error: fseek failed\n");
          return -1;
        }
        prevBlock = actBlock;
        actBlock = addr/BLOCK;
      }
      ++i;
    }
    /* we might have been at the end of the block, so a new entry
     * will need another block*/
    i = 0;
    if(addr == 0) {
      while(1) {
        block = blockVal(i, vdisk);
        if(block == -2) {
          return -1;
        }
        if(block == -1) {
          addr = BLOCK*i;
          /*write the address to keep block list updated*/
          if(fseek(vdisk, BLOCK + prevBlock*sizeof(n), SEEK_SET) != 0) {
            printf("Error: fseek failed\n");
            return -1;
          }
          n.num = addr;
          if(fwrite(&n, sizeof(n), 1, vdisk) != 1) {
            printf("Error: fwrite 3 failed\n");
            return -1;
          }
          if(fseek(vdisk, BLOCK + i*sizeof(n), SEEK_SET) != 0) {
            printf("Error: fseek failed\n");
            return -1;
          }
          n.num = 0;
          if(fwrite(&n, sizeof(n), 1, vdisk) != 1) {
            printf("Error: fwrite 4 failed\n");
            return -1;
          }
          break;
        }
        ++i;
      }
    }
    strcpy(kat.name, name);
    kat.size = fsize;
    kat.addr = faddr;
    kaddr = addr;
  }

  addr = kat.addr;
  /*copying data from file*/
  while(1) {
    if(fseek(vdisk, addr, SEEK_SET) != 0) {
      printf("Error: fseek failed\n");
      return -1;
    }
    actBlock = addr/BLOCK;
    if(fread(buffer, sizeof(char), BLOCK, fp) <= 0) {
      if(!feof(fp)) {
        printf("Error: fread 3 failed\n");
        return -1;
      }
    }
    if(fwrite(buffer, sizeof(char), BLOCK, vdisk) <= 0)  {
      if(!feof(vdisk)) {
        printf("Error: fwrite 5 failed\n");
        return -1;
      }
    }
    size -= BLOCK;
    if(size <= 0) {
      break;
    }
    i = 0;
    while(1) {
      block = blockVal(i, vdisk);
      if(block == -2) {
        return -1;
      }
      if(block == -1) {
        actBlock = addr / BLOCK;
        if(fseek(vdisk, BLOCK + actBlock*sizeof(n), SEEK_SET) != 0) {
          printf("Error: fseek failed\n");
          return -1;
        }
        n.num = addr = i*BLOCK;
        if(fwrite(&n, sizeof(n), 1, vdisk) != 1) {
          printf("Error: fwrite 6 failed\n");
          return -1;
        }
        if(fseek(vdisk, BLOCK + i*sizeof(n), SEEK_SET) != 0) {
          printf("Error: fseek failed\n");
          return -1;
        }
        n.num = 0;
        if(fwrite(&n, sizeof(n), 1, vdisk) != 1) {
          printf("Error: fwrite 7 failed\n");
          return -1;
        }
        break;
      }
      ++i;
    }
  }
  
  ++info.fcount;
  size = (fsize/BLOCK + 1) * BLOCK;
  info.free -= size;
  if(fseek(vdisk, 0, SEEK_SET) != 0) {
    printf("Error: fseek failed\n");
    return -1;
  }
  if(fwrite(&info, sizeof(info), 1, vdisk) != 1) {
    printf("Error: fwrite 8 failed\n");
    return -1;
  }
  if(fseek(vdisk, kaddr, SEEK_SET) != 0) {
    printf("Error: fseek failed\n");
    return -1;
  }
  if(fwrite(&kat, sizeof(kat), 1, vdisk) != 1) {
    printf("Error fwrite 9 failed\n");
    return -1;
  }
  
  if(fclose(fp) != 0 || fclose(vdisk) != 0) {
    printf("Error: fclose failed\n");
    return -1;
  }
  return 0;
}


/*********************************************************
* vdisk		disc file pointer
* fp		minix file pointer
* buffer	block-sized buffer
* info		disc information structure
* cat		catalogue entry structure
**********************************************************/
int v2m(char *name) {
  FILE *vdisk, *fp;
  char buffer[BLOCK];
  struct d_info info;
  struct catalogue cat;

  int i, d, fsize, faddr, addr, block;
  
  printf("copying %s to minix disk\n", name);
  vdisk = fopen("dysk", "r+b");
  if(vdisk == NULL) {
    printf("Error: cannot open disc file\n");
    return -1;
  }
  if((fp = fopen(name, "r")) != NULL) {
    fclose(fp);
    printf("Error: File already exists\n");
    return -1;
  }
  fp = fopen(name, "w");

  if(fseek(vdisk, 0, SEEK_SET) != 0) {
    printf("Error: fseek failed\n");
    return -1;
  }
  if(fread(&info, sizeof(info), 1, vdisk) != 1) {
    printf("Error: fread failed\n");
    return -1;
  }
  addr = info.first;
  if(fseek(vdisk, addr, SEEK_SET) != 0) {
    printf("Error: fseek failed\n");
    return -1;
  }
  i = 1;
  while(i <= info.fcount) {
    if(fread(&cat, sizeof(cat), 1, vdisk) != 1) {
      printf("Error: fread failed\n");
      return -1;
    }
    if(!strcmp(cat.name, name)) {
      fsize = cat.size;
      faddr = cat.addr;
      break;
    }
    if(i % (BLOCK/sizeof(cat)) == 0) {
      addr = blockVal(addr/BLOCK, vdisk);
      if(addr == -2) {
        return -1;
      }
      if(fseek(vdisk, addr, SEEK_SET) != 0) {
        printf("Error: fseek failed\n");
        return -1;
      }
    }
    ++i;
  }
  if(i > info.fcount) {
    printf("Error: No such file on disk\n");
    return -1;
  }
  addr = faddr;
  while(1) {
    if(addr == 0) break;
    if(fseek(vdisk, addr, SEEK_SET) != 0) {
      printf("Error: fseek failed\n");
      return -1;
    }
    if(fread(buffer, sizeof(char), BLOCK, vdisk) <= 0) {
      if(!feof(vdisk)) {
        printf("Error: fread failed\n");
        return -1;
      }
    }
    if(fsize > BLOCK) {
      if(fwrite(buffer, sizeof(char), BLOCK, fp) <= 0)  {
        if(!feof(fp)) {
          printf("Error: fwrite failed\n");
          return -1;
        }
      }
      fsize -= BLOCK;
    }
    else {
      if(fwrite(buffer, sizeof(char), fsize, fp) <= 0)  {
        if(!feof(fp)) {
          printf("Error: fwrite failed\n");
          return -1;
        }
      }
      break;
    }
    addr = blockVal(addr/BLOCK, vdisk);
    if(addr == -2) {
      return -1;
    }
  }
  if(fclose(fp) != 0|| fclose(vdisk) != 0) {
    printf("Error: fclose failed\n");
    return -1;
  }
  return 0;
}

