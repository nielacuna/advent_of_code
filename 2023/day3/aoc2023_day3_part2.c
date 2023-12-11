#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>


enum {
        TOKEN_LITERAL = 128,
};

char *map;
static int tk;
static int tk_value;
static char *p;
static char *pos;
static int gear_idx;
static int linesize;
static int count;

static const char gear = '*';

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

static bool is_gear(const int idx)
{
        char *pp = map + idx;
        if (*pp == gear) {
                gear_idx = idx;
                return true;
        }
        return false;
}

static bool is_gear_at_idx_backwards(const int idx)
{
        if (idx > 0) {
                return is_gear(idx);
        }
        return false;
}

/* need to determine array length here */
static bool is_gear_at_idx_forwards(const int idx)
{
        if (idx < count) {
                return is_gear(idx);
        }
        return false;
}

static bool top_left_is_gear(const int idx)
{
        return is_gear_at_idx_backwards(idx - linesize - 1);
}

static bool top_is_gear(const int idx)
{
        return is_gear_at_idx_backwards(idx - linesize);
}

static bool top_right_is_gear(const int idx)
{
        return is_gear_at_idx_backwards(idx - linesize + 1);
}

static bool left_is_gear(const int idx)
{
        return is_gear_at_idx_backwards(idx - 1);
}

static bool right_is_gear(const int idx)
{
        return is_gear_at_idx_forwards(idx + 1);
}

static bool bottom_left_is_gear(const int idx)
{
        return is_gear_at_idx_forwards(idx + linesize - 1);
}

static bool bottom_is_gear(const int idx)
{
        return is_gear_at_idx_forwards(idx + linesize);
}

static bool bottom_right_is_gear(const int idx)
{
        return is_gear_at_idx_forwards(idx + linesize + 1);
}

static bool part_number(void)
{
        char *pp = pos;
        char *ppp = pos;

        while(*pp >= '0' && *pp <= '9') {
                int idx = (int)(pp - map);

                /* check top left of digit */
                if (top_left_is_gear(idx)) {
                        return true;
                }
                
                /* check top of digit */
                if (top_is_gear(idx)) {
                        return true;
                }

                /* check top right of digit */
                if (top_right_is_gear(idx)) {
                        return true;
                }

                /* check left of digit, if possible */
                if (left_is_gear(idx)) {
                        return true;
                }

                /* check right of digit, if possible */
                if(right_is_gear(idx)) {
                        return true;
                }

                /* check bottom left of digit */
                if (bottom_left_is_gear(idx)) {
                        return true;
                }

                /* check bottom of digit */
                if (bottom_is_gear(idx)) {
                        return true;
                }

                /* check bottom right of digit */
                if (bottom_right_is_gear(idx)) {
                        return true;
                }
                pp++;
        }
        return false;
}

#define PN_PER_GEAR     2 /* part numbers per gear allowed */
struct gear_struct {
        size_t igear; /* index of gear in buf */
        int pn[PN_PER_GEAR + 1];
        int ipn;
        struct gear_struct *next;
};

static struct gear_struct *list = NULL;

static struct gear_struct *alloc_new_gear(void)
{
        struct gear_struct *g;
        if (!(g = malloc(sizeof(*g)))) {
                fprintf(stderr, "cannot allocate memory for new gear\n");
                exit(-1);
        }
        g->igear = gear_idx;
        g->ipn = 0;
        g->next = NULL;
        return g;
}

static void add_pn_to_gear(struct gear_struct *g, const int pn)
{
        if (g->ipn < (PN_PER_GEAR + 1)) {
                g->pn[g->ipn] = pn;
                g->ipn++;
        }
        return;
}

static void new_part_number(void)
{
        struct gear_struct **listpp = &list;

        while(*listpp) {
                struct gear_struct *gearp = *listpp;
                if (gearp->igear == gear_idx) {
                        break;
                }
                listpp = &gearp->next;
        }

        if (*listpp == NULL) {
                /* create new gear if new */
                *listpp = alloc_new_gear();
        }

        /* add the part number to this gear */
        add_pn_to_gear(*listpp, tk_value);
        return;
}

int main(void)
{
        int fd;
        struct stat st;
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
                if (tk_value == 489)
                {
                        printf("\n");
                }
                if (part_number()) {
                        //printf("pn %d is adjacent to a gear\n", tk_value);
                        new_part_number();
                } 
                next();
        }

        struct gear_struct *g = list;
        while (g) {
                int ratio = g->pn[0] * g->pn[1];
                printf("gear1: %d, gear2: %d, ratio :%d ", g->pn[0], g->pn[1], ratio);
                /* check if we have the requisite number of gear 
                 * before getting the gear ratio */
                if (g->ipn == PN_PER_GEAR) {
                        ans += ratio;
                        printf("OK!\n");
                } else {
                        printf("~\n");
                }
                g = g->next;
        }

        printf("ans is %d\n", ans);
        free(buf);
        close(fd);
        return 0;
}

