#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "fs.h"

int main(int argc, char* argv[]) {
  int i=0;
  if(argc<2) {
    printf("usage:\t name -(cvmlsrd) [file name]\n");
    return -1;
  }
  switch(argv[1][1]) {
    case 'c' :
               if(argc != 3 || atoi(argv[2]) == 0) {
                 return -1;
               }
               return vdiskCreate( atoi(argv[2]) );
    case 'v' :
               if(argc != 3) {
                 return -1;
               }
               return m2v(argv[2]);
    case 'm' :
               if(argc != 3) {
                 return -1;
               }
               return v2m(argv[2]);
    case 'l' : 
               return ls();
    case 's' : 
               return disc_stat();
    case 'd' : 
               return del_disc();
    case 'r' : 
               if(argc != 3) {
                 return -1;
               }
               return fremove(argv[2]);
    default : 
               return -1;
  }
  return 0;
}

