#include "aoc.h"
#include <regex.h>
#include <stdio.h>

const char patterns[] = "-?[0-9]+";

struct vector {
        int64_t size;
        int64_t arr[MAX_LEN];
};

static void push(struct vector *v, int64_t e) {
        assert(v->size < MAX_LEN);
        v->arr[v->size++] = e;
}

static int64_t pop_back(struct vector *v) {
        assert(v);
        assert(v->size > 0);
        return v->arr[--v->size];
}

static bool equal_zero(int64_t n) { return n == 0; }

static bool all(struct vector *v, bool (*pred)(int64_t)) {
        for (int64_t i = 0; i < v->size; i++)
                if (!pred(v->arr[i]))
                        return false;
        return true;
}

static int64_t front(struct vector *v) {
        assert(v);
        assert(v->size > 0);
        return v->arr[0];
}

// #define DDEBUG
#ifdef DDEBUG
#define print(v)                                                               \
        do {                                                                   \
                for (int64_t __i = 0; __i < (v)->size; __i++)                  \
                        printf("%10ld", (v)->arr[__i]);                        \
                printf("\n");                                                  \
        } while (0)
#define log(x, ...) printf(x, __VA_ARGS__)
#else
#define print(v)
#define log(x, ...)
#endif

static char *process_line(char *line, regex_t *regex, struct result *out) {
        char         *err  = NULL;
        struct vector nums = {0}, fronts = {0};
        regmatch_t    match;
        int64_t       end = strlen(line), i = 0, res_a = 0, res_b = 0;

        while (i < end &&
               regexec(regex, line + i, 1, &match, 0) != REG_NOMATCH) {
                push(&nums, atoi(line + i + match.rm_so));
                i += match.rm_eo + 1;
        }

        print(&nums);

        while (!all(&nums, equal_zero)) {
                push(&fronts, front(&nums));
                for (i = 0; i < nums.size - 1; i++)
                        nums.arr[i] = nums.arr[i + 1] - nums.arr[i];
                res_a += pop_back(&nums);
                print(&nums);
        }

        for (i = 0; i < fronts.size; i++)
                res_b = fronts.arr[fronts.size - 1 - i] - res_b;

        log("%ld %ld\n", res_a, res_b);

        out->a += res_a;
        out->b += res_b;

        return err;
}

const char *run(FILE *f, struct result *out) {
        char       *line = NULL;
        const char *err  = NULL;
        size_t      line_len;
        regex_t     regex;

        if (regcomp(&regex, patterns, REG_EXTENDED) != 0) {
                err = "Regex compilation failed\n";
                goto regcomp_error;
        }

        while (getline(&line, &line_len, f) != -1)
                if ((err = process_line(line, &regex, out)) != NULL)
                        goto error;

error:
        free(line);
        regfree(&regex);
regcomp_error:
        return err;
}
