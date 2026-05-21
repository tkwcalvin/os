#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void sixfive(int fd);
void match(char *p);
char buf[1024];

int
main(int argc, char *argv[])
{
    int fd, i;

    if (argc <= 1){
        fprintf(2, "Usage: sixfive <file1> <file2>...\n");
        exit(1);
    }

    for (i = 1; i < argc; i++){
        if ((fd = open(argv[i], O_RDONLY)) < 0){
            printf("sixfive: cannot open %s\n", argv[i]);
            exit(1);
        }
        sixfive(fd);
        close(fd);
    }
    exit(0);
}

void
sixfive(int fd)
{
    int n, m;
    char *p, *q;

    m = 0;
    while ((n = read(fd, buf+m, sizeof(buf)-m-1)) > 0){
        m += n;
        buf[m] = '\0';
        p = buf;
        while ((q = strchr(p, '\n')) != 0){
            *q = 0;
            match(p);
            *q = '\n';
            p = q+1;
        }
        match(p);
    }
}

static int
issep(char c){
    return strchr(" \r\t\n\v-_./,", c) != 0;
}

void match(char *p){
    char *left = p;
    int is_accum = 1;
    int accum = 0;


    for (;*p!='\0';p++){
        if (*p >= '0' && *p <= '9'){
            if (accum == 0){
                left = p;
            }
            accum = accum * 10 + (*p - '0');
        }else if (issep(*p)){
            if (is_accum && accum > 0 &&(accum % 5 == 0||accum % 6 == 0)){
                write(1, left, p - left);
                write(1, "\n", 1);
            }
            is_accum = 1;
            accum = 0;
        }else {
            is_accum = 0;
            accum = 0;
        }
    }

    if (is_accum && accum > 0 &&(accum % 5 == 0||accum % 6 == 0)){
        write(1, left, p - left);
        write(1, "\n", 1);
    }
}