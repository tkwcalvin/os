#include "kernel/types.h"
#include "user/user.h"


#define MLEN 1024*16

int
main(int argc, char *argv[])
{
  char *c = sbrk(MLEN);
  if(c == (char*)-1){
    exit(1);
  }

  char *hint = "This may help.";
  int hlen = strlen(hint);

  for(int i = 0; i < MLEN - hlen; i++){
    if(memcmp(c + i, hint, hlen) == 0){
      int j = i + 16;
      while(j < MLEN && c[j] != 0){
        printf("%c", c[j]);
        j++;
      }
      if(j > i + 16)
        printf("\n");
      exit(0);
    }
  }

  exit(0);
}
