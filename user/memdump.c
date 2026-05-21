#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}


static char digits[] = "0123456789ABCDEF";

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
}

static void
printint(int fd, long long xx, int base, int sgn)
{
  char buf[20];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(--i >= 0)
    putc(fd, buf[i]);
}

void
memdump(char *fmt, char *data)
{

  // Your code here.
  void *p = (void *)data;
  for (;*fmt!='\0';fmt++){
    if (*fmt == 'i'){
      printf("%d\n", *(int *)p);
      p += sizeof(int);
    }else if (*fmt == 'p'){
      printint(1, *(uint64*)p, 16, 1);
      printf("\n");
      p += sizeof(uint64);
    }
    else if (*fmt == 'c'){
      printf("%c\n", *(char *)p);
      p += sizeof(char);
    }
    else if (*fmt == 's'){
      printf("%s\n", *(char **)p);
      p += sizeof(char *);
    }
    else if (*fmt == 'S'){
      printf("%s\n", (char *)p);
      p += strlen((char *)p) + 1;
    }
    else if (*fmt == 'h'){
      printint(1, *(short *)p, 10, 1);
      printf("\n");
      p += sizeof(short);
    }
    
  }

}
