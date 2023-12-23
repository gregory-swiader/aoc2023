#include "aoc.h"
#include <regex.h>

#define MAX_LEN   4096
#define COUNT     1
#define COLOUR    2
#define SEP       3
#define NR_GROUPS 4
const char GAME_NR[]   = "Game ([0-9]+):";
const char ROUND_RES[] = " ([0-9]+) ([a-z]+)([,;])?";

struct colours {
        int64_t red, green, blue;
};
const struct colours limit = {.red = 12, .green = 13, .blue = 14};

static int64_t is_allowed(struct colours *col) {
        return col->red <= limit.red && col->green <= limit.green &&
               col->blue <= limit.blue;
}

static void assign(struct colours *counts, struct colours *minim,
                   int64_t *lost) {
        minim->red   = MAX(minim->red, counts->red);
        minim->green = MAX(minim->green, counts->green);
        minim->blue  = MAX(minim->blue, counts->blue);
        *lost        = !*lost ? !is_allowed(counts) : *lost;
}

static int64_t process_round(char *round, regex_t *reg_round, int64_t *lost,
                             struct colours *minim) {
        regmatch_t     match[NR_GROUPS] = {};
        struct colours counts           = {};
        int            i;

        for (i = 0; i < 3; i++) {
                if (regexec(reg_round, round, NR_GROUPS, match, 0) != 0) {
                        fprintf(stderr, "Panic! there is no round match\n");
                        return -1;
                }

                if (strcmp_prefix(round + match[COLOUR].rm_so, "green") == 0)
                        counts.green = atoi(round + match[COUNT].rm_so);
                if (strcmp_prefix(round + match[COLOUR].rm_so, "blue") == 0)
                        counts.blue = atoi(round + match[COUNT].rm_so);
                if (strcmp_prefix(round + match[COLOUR].rm_so, "red") == 0)
                        counts.red = atoi(round + match[COUNT].rm_so);

                if (match[SEP].rm_so == -1 || round[match[SEP].rm_so] == ';')
                        break;
                round += match[SEP].rm_eo;
        }

        assign(&counts, minim, lost);

        // printf("red: %ld, green: %ld, blue: %ld %s\n", counts.red,
        // counts.green, counts.blue, is_allowed(&counts) ? "GOOD" : "BAD");

        return 0;
}

static char *process_line(char *line, regex_t *reg_game, regex_t *reg_round,
                          struct result *out) {
        regmatch_t match_game[2];
        char      *round = line;

        struct colours minim = {};
        int64_t        lost  = 0;

        if (regexec(reg_game, line, 2, match_game, 0) != 0)
                return "Panic! There is no game number match!\n";

        do {
                if (process_round(++round, reg_round, &lost, &minim)) {
                        return "Panic! There is no match for a round!\n";
                }
                // printf("red: %ld, green: %ld, blue: %ld\n", minim.red,
                //        minim.green, minim.blue);
        } while ((round = strchr(round, ';')) != NULL);

        out->b += minim.red * minim.green * minim.blue;
        out->a += lost ? 0 : atoi(line + match_game[1].rm_so);
        return 0;
}

char *run(FILE *f, struct result *out) {
        regex_t game_reg, round_reg;
        char    line[MAX_LEN], *err = NULL;

        if (regcomp(&game_reg, GAME_NR, REG_EXTENDED) != 0) {
                err = "Regex compilation failed\n";
                goto regcomp_error;
        }

        if (regcomp(&round_reg, ROUND_RES, REG_EXTENDED) != 0) {
                err = "Regex compilation failed\n";
                goto regcomp_error_cleanup;
        }

        while (fgets(line, sizeof(line), f) != NULL) {
                if (process_line(line, &game_reg, &round_reg, out) != 0) {
                        err = "Processing a line";
                        goto cleanup;
                }
        }

cleanup:
        regfree(&round_reg);
regcomp_error_cleanup:
        regfree(&game_reg);
regcomp_error:
        return err;
}
