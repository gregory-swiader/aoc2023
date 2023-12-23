#include "aoc.h"
#include <regex.h>
#include <unistd.h>

const char patterns_a[] = "[0-9]";
const char patterns_b[] = "[0-9]|one|two|three|four|five|six|seven|eight|nine";

static int strcmp_prefixed(char *s1, char *s2) {
        return strncmp(s1, s2, strlen(s2));
}

static int to_num(char *word) {
        if (strcmp_prefixed(word, "one") == 0)
                return 1;
        if (strcmp_prefixed(word, "two") == 0)
                return 2;
        if (strcmp_prefixed(word, "three") == 0)
                return 3;
        if (strcmp_prefixed(word, "four") == 0)
                return 4;
        if (strcmp_prefixed(word, "five") == 0)
                return 5;
        if (strcmp_prefixed(word, "six") == 0)
                return 6;
        if (strcmp_prefixed(word, "seven") == 0)
                return 7;
        if (strcmp_prefixed(word, "eight") == 0)
                return 8;
        if (strcmp_prefixed(word, "nine") == 0)
                return 9;

        return word[0] - '0';
}

static int convert(char *line, regmatch_t first, regmatch_t last) {
        return to_num(line + first.rm_so) * 10 + to_num(line + last.rm_so);
}

static char *process_line(char *line, regex_t *regex, int64_t *out) {
        regmatch_t tmp, first = {-1, -1}, last = {-1, -1};
        int        end = strlen(line), pos = 0;

        if (regexec(regex, line + pos, 1, &tmp, 0) == REG_NOMATCH)
                return "Panic! there is no first match\n";

        first = tmp;

        do {
                last.rm_so  = pos + tmp.rm_so;
                last.rm_eo  = pos + tmp.rm_eo;
                pos        += tmp.rm_so + 1;
        } while (pos < end &&
                 regexec(regex, line + pos, 1, &tmp, 0) != REG_NOMATCH);

        // write(STDOUT_FILENO, line, end);
        // write(STDOUT_FILENO, line + first.rm_so, first.rm_eo - first.rm_so);
        // write(STDOUT_FILENO, " ", 1);
        // write(STDOUT_FILENO, line + last.rm_so, last.rm_eo - last.rm_so);
        // write(STDOUT_FILENO, "\n", 1);

        *out += convert(line, first, last);
        return NULL;
}

char *run(FILE *f, struct result *out) {
        regex_t regex_a, regex_b;
        char    line[MAX_LEN], *err = NULL;

        if (regcomp(&regex_a, patterns_a, REG_EXTENDED) != 0) {
                err = "Regex compilation failed\n";
                goto regcomp_error;
        }

        if (regcomp(&regex_b, patterns_b, REG_EXTENDED) != 0) {
                err = "Regex compilation failed\n";
                goto regcomp_error_cleanup;
        }

        while (fgets(line, sizeof(line), f) != NULL) {
                if ((err = process_line(line, &regex_a, &out->a)) != NULL)
                        goto cleanup;
                if ((err = process_line(line, &regex_b, &out->b)) != NULL)
                        goto cleanup;
        }

cleanup:
        regfree(&regex_a);
regcomp_error_cleanup:
        regfree(&regex_b);
regcomp_error:
        return err;
}
