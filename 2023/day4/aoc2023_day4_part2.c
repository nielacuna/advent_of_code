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
static char *cardp;
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
                        cardp = p - 1;
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

#define WINNING_NUMBERS_COUNT   10

static int winning_numbers[WINNING_NUMBERS_COUNT];

static int scratch_cards_count[256] = {0};

int main(void)
{
        int fd;
        struct stat st;
        char *buf;
        char *pp;
        int ans = 0;
        int iterations = 0;
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

        /* we process each scratch card at least once */
        for (size_t i = 0; i<256; i++) {
                scratch_cards_count[i] = 1;
        }

        next();
        while (tk != '\0') {
                int *scratchcard_count_ptr;
                assert(tk == TOKEN_CARD);

                /* card number follows "Card" */
                next();
                assert(tk == TOKEN_LITERAL);
                card_n = tk_value;
                scratchcard_count_ptr = scratch_cards_count + (card_n - 1);

                /* next should be a colon */
                next();
                assert(tk == ':');

                /* next n literals are the winning numbers */
                size_t iwinning = 0;
                next();
                while(tk != '|') {
                        assert(tk == TOKEN_LITERAL);
                        assert(iwinning < WINNING_NUMBERS_COUNT);
                        winning_numbers[iwinning] = tk_value;
                        iwinning++;
                        next();
                };

                /* next set of literals are the actual numbers we have */
                next();
                size_t wins = 0;
                int *next_scratchcard_count_ptr = scratch_cards_count + card_n;
                while(tk != '\n') {
                        size_t i;
                        assert(tk == TOKEN_LITERAL);
                        for (i=0; i<WINNING_NUMBERS_COUNT; i++) {
                                if (winning_numbers[i] == tk_value) {
                                        wins++;
                                        *next_scratchcard_count_ptr = *next_scratchcard_count_ptr + 1;
                                        next_scratchcard_count_ptr++;
                                }
                        }

                        /* get next literal on our scratch card */
                        next();
                }

                /* repeat processing of the same card */
                if (++iterations < *scratchcard_count_ptr) {
                        p = cardp;
                } else {
                        ans += *scratchcard_count_ptr;
                        iterations = 0;
                }

                /* get next "Card" token */
                next();
        }
        printf("ans is %d\n", ans);

        /* we can now free our initial mmap */
        munmap(map, st.st_size);
        close(fd);
        return 0;
}

