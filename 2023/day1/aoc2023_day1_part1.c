#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

enum {
        TOKEN_LITERAL = 128,
        TOKEN_STRING,
};

char *map;
static int tk;
static int tk_value;
static char *p;

static void next(void)
{
        char *name;
        char *pp;
        while(tk = *p) {
                p++;
                if (tk >= '0' && tk <= '9') {
                        tk_value = tk - '0';
                        tk = TOKEN_LITERAL;
                        return;
                }
                switch(tk) {
                case '\n':
                        return;
                }
        }
        return;
}

int main(void)
{
        int fd;
        struct stat st;
        int calibration;
        if ((fd = open("input", O_RDONLY)) == -1) {
                fprintf(stderr, "cannot open input file\n");
                exit(-1);
        }
        fstat(fd, &st);
        if ((map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED) {
                fprintf(stderr, "cannot mmap input file\n");
                exit(-1);
        }
        p = map;

        tk = '\n';
        int ans = 0;
        while(tk) {
                /* get the first number */
                next();
                if (tk == 0)
                        break;
                calibration = tk_value;

                /* get the last number */
                while(tk != '\n') {
                        next();
                }
                calibration = (calibration * 10) + tk_value;
                ans = ans + calibration;
        }

        printf("ans is %d\n", ans);
        munmap(map, st.st_size);
        return 0;
}

