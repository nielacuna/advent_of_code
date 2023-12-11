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
        TOKEN_CARD,
};

char *map;
static int tk;
static int tk_value;
static char *p;
static int card_n;

static void next(void)
{
        char *name;
        char *pp;
        while(tk = *p) {
                p++;
                switch(tk) {
                case '0': case '1': case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                        tk_value = tk - '0';
                        while(*p >= '0' && *p <= '9') {
                                tk_value = (tk_value * 10) + (*p - '0');
                                p++;
                        }
                        tk = TOKEN_LITERAL;
                        return;
                }
                switch (tk) {
                case 'C': /* assume Card */
                        p = p + strlen("ard");
                        tk = TOKEN_CARD;
                        return;
                case '|':
                case ':':
                case '\n':
                        return;
                }
        }
        return;
}

static bool is_symbol(const int idx)
{
        char *pp = map + idx;
        return (*pp != '.') && (*pp < '0' || *pp > '9');
}

static bool is_symbol_at_idx_backwards(const int idx)
{
        if (idx > 0) {
                return is_symbol(idx);
        }
        return false;
}

#define WINNING_NUMBERS_COUNT   10

static int winning_numbers[WINNING_NUMBERS_COUNT];

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

        next();
        while (tk != '\0') {
                assert(tk == TOKEN_CARD);

                /* card number follows "Card" */
                next();
                assert(tk == TOKEN_LITERAL);
                card_n = tk_value;

                /* next should be a colon */
                next();
                assert(tk == ':');

                /* next n literals is our winning numbers */
                size_t iwinning = 0;
                next();
                while(tk != '|') {
                        assert(tk == TOKEN_LITERAL);
                        assert(iwinning < WINNING_NUMBERS_COUNT);
                        winning_numbers[iwinning] = tk_value;
                        iwinning++;
                        next();
                };

                next();
                size_t power_of_two = 0;
                while(tk != '\n') {
                        size_t i;
                        assert(tk == TOKEN_LITERAL);
                        for (i=0; i<WINNING_NUMBERS_COUNT; i++) {
                                if (winning_numbers[i] == tk_value) {
                                        power_of_two++;
                                }
                        }
                        next();
                }
                if (power_of_two) {
                        ans += (1 << (power_of_two-1));
                }
                next();
        }
        printf("ans is %d\n", ans);

        /* we can now free our initial mmap */
        munmap(map, st.st_size);
        close(fd);
        return 0;
}

