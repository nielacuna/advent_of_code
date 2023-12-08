#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *first_digit(char *line)
{
        char *ptr;
        for (ptr = line; *ptr != '\0'; ptr++) {
                if (*ptr >= '0' && *ptr <= '9') {
                        return ptr;
                }
        }
        return NULL;
}

static int line_calibration_value(char *line)
{
        char *ptr = line;
        int calibration = 0;

        if (ptr = first_digit(ptr)) {
                int last = calibration = *ptr - '0';
                ptr++;
                while(ptr = first_digit(ptr)) {
                        last = *ptr - '0';
                        ptr++;
                }
                calibration = (calibration * 10) + last;
        }
        return calibration;
}

int main(void)
{
        FILE *in;
        char linebuf[1024];
        int ans = 0;

        if ((in = fopen("./input", "r")) == NULL) {
                fprintf(stderr, "input file not found in CWD\n");
                exit(-1);
        }

        while(fgets(linebuf, sizeof(linebuf), in)) {
                int tmp;
                tmp = line_calibration_value(linebuf);
                printf("tmp(%d): %s", tmp, linebuf);
                ans = ans+tmp;
        }
        printf("answer is %d\n", ans);
        return 0;
}

