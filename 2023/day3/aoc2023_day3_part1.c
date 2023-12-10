#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>
#include <stdbool.h>

#define BUFFER_BLOCKSIZE       256

enum {
        TOKEN_LITERAL = 128,
};

char *map;
static int tk;
static int tk_value;
static char *p;
static char *pos;
static int linesize;
static int count;
static char sym;

/* this time we are only interested in digits */
static void next(void)
{
        char *name;
        char *pp;
        while(tk = *p) {
                p++;
                switch(tk) {
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                        pos = p - 1;
                        tk_value = tk - '0';
                        while(*p >= '0' && *p <= '9') {
                                tk_value = (tk_value * 10) + (*p - '0');
                                p++;
                        }
                        tk = TOKEN_LITERAL;
                        return;
                }
        }
        return;
}

static bool is_symbol(const int idx)
{
        char *pp = map + idx;
        sym = *pp;
        return (*pp != '.') && (*pp < '0' || *pp > '9');
}

static bool is_symbol_at_idx_backwards(const int idx)
{
        if (idx > 0) {
                return is_symbol(idx);
        }
        return false;
}

/* need to determine array length here */
static bool is_symbol_at_idx_forwards(const int idx)
{
        if (idx < count) {
                return is_symbol(idx);
        }
        return false;
}

static bool top_left_is_symbol(const int idx)
{
        return is_symbol_at_idx_backwards(idx - linesize - 1);
}

static bool top_is_symbol(const int idx)
{
        return is_symbol_at_idx_backwards(idx - linesize);
}

static bool top_right_is_symbol(const int idx)
{
        return is_symbol_at_idx_backwards(idx - linesize + 1);
}

static bool left_is_symbol(const int idx)
{
        return is_symbol_at_idx_backwards(idx - 1);
}

static bool right_is_symbol(const int idx)
{
        return is_symbol_at_idx_forwards(idx + 1);
}

static bool bottom_left_is_symbol(const int idx)
{
        return is_symbol_at_idx_forwards(idx + linesize - 1);
}

static bool bottom_is_symbol(const int idx)
{
        return is_symbol_at_idx_forwards(idx + linesize);
}

static bool bottom_right_is_symbol(const int idx)
{
        return is_symbol_at_idx_forwards(idx + linesize + 1);
}

static bool part_number(void)
{
        char *pp = pos;
        char *ppp = pos;

        while(*pp >= '0' && *pp <= '9') {
                int idx = (int)(pp - map);

                /* check top left of digit */
                if (top_left_is_symbol(idx)) {
                        return true;
                }
                
                /* check top of digit */
                if (top_is_symbol(idx)) {
                        return true;
                }

                /* check top right of digit */
                if (top_right_is_symbol(idx)) {
                        return true;
                }

                /* check left of digit, if possible */
                if (left_is_symbol(idx)) {
                        return true;
                }

                /* check right of digit, if possible */
                if(right_is_symbol(idx)) {
                        return true;
                }

                /* check bottom left of digit */
                if (bottom_left_is_symbol(idx)) {
                        return true;
                }

                /* check bottom of digit */
                if (bottom_is_symbol(idx)) {
                        return true;
                }

                /* check bottom right of digit */
                if (bottom_right_is_symbol(idx)) {
                        return true;
                }
                pp++;
        }
        return false;
}

int main(void)
{
        int fd;
        struct stat st;
        int calibration;
        char *buf;
        char *pp;
        int ans = 0;
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

        /* get line size */
        for (p = map; *p != '\n'; p++);
        linesize = p - map;

        /* duplicate buffer and remove the newlines on copy */
        if (!(pp = buf = malloc(st.st_size))) {
                fprintf(stderr, "cannot allocate memory\n");
                exit(-1);
        }
        p = map;
        while (*p != '\0') {
                if (*p != '\n') {
                        *pp = *p;
                        pp++;
                } 
                p++;
        }

        /* we can now free our initial mmap */
        munmap(map, st.st_size);
        p = buf;
        map = buf;
        count = pp - buf;

        next();
        while (tk != '\0') {
                if (part_number()) {
                        ans += tk_value;
                } 
                next();
        }
        printf("ans is %d\n", ans);
        return 0;
}

