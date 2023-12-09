#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <assert.h>
#include <stdbool.h>

enum {
        TOKEN_LITERAL = 128,
        TOKEN_STRING,
        TOKEN_GAME,
        TOKEN_COLOR,
};

enum {
        red = 0,
        green,
        blue,
        rgb_max,
};

static size_t rgb_max_cubes[rgb_max];

static const char *rgb_cubes_name[rgb_max] = {
        "red", "green", "blue",
};

char *map;
static int tk;
static int tk_value;
static char *p;
static int igame;
static bool game_is_possible;

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
                case 'G': /* G assumes "Game" */
                        p = p + strlen("ame");
                        tk = TOKEN_GAME;
                        return;
                case 'b': /* assume blue */
                        p = p + strlen("lue");
                        tk_value = blue;
                        tk = TOKEN_COLOR;
                        return;
                case 'r': /* assume red */
                        p = p + strlen("ed");
                        tk_value = red;
                        tk = TOKEN_COLOR;
                        return;
                case 'g': /* assume green */
                        p = p + strlen("reen");
                        tk_value = green;
                        tk = TOKEN_COLOR;
                        return;
                case ':':
                case ',':
                case ';':
                        return;
                case '\n':
                        return;
                }
        }
        return;
}

static void game_expr(void)
{
        game_is_possible = false;

        memset(rgb_max_cubes, 0, sizeof rgb_max_cubes);

        /* a line always starts with "Game" */
        assert(tk == TOKEN_GAME);

        /* next token is the game index  */
        next();
        assert(tk == TOKEN_LITERAL);
        igame = tk_value;

        /* followed by a colon */
        next();
        assert(tk == ':');

        //printf("found Game (%d):\n", igame);

        /* now we parse the RGB cubes which follows the following template
         * per set:
         *
         * note to self: put the grammar rule in this comment
         * */
        next();
        while(tk != '\n') {
                assert(tk == TOKEN_LITERAL);

                /* save the value, we still don't know the color at this point */
                int count = tk_value;
                
                next();
                assert(tk == TOKEN_COLOR);
                //printf("\tfound: %d %s\n", count, rgb_cubes_name[tk_value]);

                /* update the max for the cube */
                if (count > rgb_max_cubes[tk_value]) {
                        rgb_max_cubes[tk_value] = count;
                }

                next();
                if (tk == ';' || tk == ',') {
                        next();
                }
        }
        game_is_possible = true;
       
        return;
}

int main(void)
{
        int fd;
        struct stat st;
        int calibration;
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
                int power;
                game_expr();
                power = rgb_max_cubes[0] * rgb_max_cubes[1] * rgb_max_cubes[2];
                ans += power;
                next();
        }
        printf("ans is %d\n", ans);
        munmap(map, st.st_size);
        return 0;
}

