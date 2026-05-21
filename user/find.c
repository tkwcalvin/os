#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"


char*
basename(char *path)
{
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  return p + 1;
}

void
find(char *path, char *name, int execArgc, char *execArgv[])
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    if (strcmp(name, basename(path)) == 0){
      if (execArgc > 0){
        int pid;
        if ((pid = fork()) < 0){
          fprintf(2, "fork failed\n");
          return;
        }else if (pid == 0){
          execArgv[execArgc] = path;
          execArgv[execArgc + 1] = 0;
          if (exec(execArgv[0], execArgv) < 0){
            fprintf(2, "exec %s failed\n", execArgv[0]);
            exit(1);
          }
        }else{
          wait(0);
        } 
      }else{
        printf("%s\n", path);
      }
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
    
      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      find(buf, name, execArgc, execArgv);
    }
  }
  close(fd);
}

#define MAXARG 32
int
main(int argc, char *argv[])
{

  char *execArgv[MAXARG];
  for (int i = 4; i < argc; i++){
    execArgv[i - 4] = argv[i];
  }  
  find(argv[1], argv[2], argc - 4, execArgv);
  exit(0);
}
