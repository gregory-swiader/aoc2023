#ifndef AOC_H
#define AOC_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 4096

#define MAX(a, b)                                                              \
        ({                                                                     \
                __typeof__(a) _a = (a);                                        \
                __typeof__(b) _b = (b);                                        \
                _a > _b ? _a : _b;                                             \
        })

__attribute_maybe_unused__ static int64_t strcmp_prefix(char *s1, char *s2) {
        return strncmp(s1, s2, strlen(s2));
}

struct result {
        int64_t a, b;
};

char *run(FILE *f, struct result *out);

#endif // AOC_H
