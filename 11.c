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

struct result {
        int64_t a, b;
};

struct point {
        int64_t x, y;
};

struct vector {
        int64_t       size, __size;
        struct point *arr;
};

typedef bool (*compare)(struct point, struct point);

static void push_back(struct vector *v, struct point e) {
        if (v->__size == 0)
                v->__size = 1 << 8;
        while (v->__size <= v->size + 1)
                v->__size *= 2;
        v->arr = realloc(v->arr, sizeof(*v->arr) * v->__size);

        v->arr[v->size++] = e;
}

static void copy(struct vector *restrict dest, struct vector *restrict src) {
        memcpy(dest, src, sizeof(*dest));
        dest->arr = malloc(src->__size * sizeof(*src->arr));
        memcpy(dest->arr, src->arr, src->size * sizeof(*src->arr));
}

static void clear(struct vector *v) {
        free(v->arr);
        memset(v, 0, sizeof(*v));
}

static int64_t count_where(struct vector *v, struct point e, compare eq) {
        int64_t i, sum = 0;
        for (i = 0; i < v->size; i++)
                if (eq(v->arr[i], e))
                        sum++;
        return sum;
}

static bool eq_x(struct point p, struct point q) { return p.x == q.x; }
static bool eq_y(struct point p, struct point q) { return p.y == q.y; }
static bool less_x(struct point p, struct point q) { return p.x < q.x; }
static bool less_y(struct point p, struct point q) { return p.y < q.y; }

static void find_free(struct vector *galaxies, struct vector *rows_out,
                      struct vector *cols_out) {
        struct point max_bounds = {};
        int64_t      i;

        for (i = 0; i < galaxies->size; i++) {
                max_bounds.x = MAX(max_bounds.x, galaxies->arr[i].x);
                max_bounds.y = MAX(max_bounds.y, galaxies->arr[i].y);
        }

        for (i = 0; i < MAX(max_bounds.x, max_bounds.y); i++) {
                if (count_where(galaxies, (struct point){i, i}, eq_x) == 0)
                        push_back(rows_out, (struct point){i, i});
                if (count_where(galaxies, (struct point){i, i}, eq_y) == 0)
                        push_back(cols_out, (struct point){i, i});
        }
}

static void expand_galaxies(struct vector *galaxies, struct vector *out,
                            int64_t rate) {
        struct vector free_rows = {}, free_columns = {};
        int64_t       i;

        find_free(galaxies, &free_rows, &free_columns);
        copy(out, galaxies);

        for (i = 0; i < galaxies->size; i++) {
                out->arr[i].x +=
                        count_where(&free_rows, galaxies->arr[i], less_x) *
                        (rate - 1);
                out->arr[i].y +=
                        count_where(&free_columns, galaxies->arr[i], less_y) *
                        (rate - 1);
        }
}

static int64_t distance(struct point a, struct point b) {
        return labs(a.x - b.x) + labs(a.y - b.y);
}

static void find_distances(struct vector *galaxies, int64_t *out) {
        int64_t sum = 0, i, j;

        for (i = 0; i < galaxies->size; i++)
                for (j = i + 1; j < galaxies->size; j++)
                        sum += distance(galaxies->arr[i], galaxies->arr[j]);

        *out = sum;
}

static void get_input(FILE *f, struct vector *galaxies) {
        char    line[MAX_LEN];
        int64_t i = 0, j;

        while (fgets(line, sizeof(line), f) != NULL) {
                for (j = 0; j < strlen(line); j++)
                        if (line[j] == '#')
                                push_back(galaxies, (struct point){i, j});
                i++;
        }
}

static void run(FILE *f, struct result *out) {
        struct vector galaxies = {}, galaxies_a, galaxies_b;

        get_input(f, &galaxies);
        expand_galaxies(&galaxies, &galaxies_a, 2);
        expand_galaxies(&galaxies, &galaxies_b, 1000000);
        find_distances(&galaxies_a, &out->a);
        find_distances(&galaxies_b, &out->b);

        clear(&galaxies);
}

int32_t main(int32_t argc, char *argv[]) {
        FILE         *f   = stdin;
        char         *err = NULL;
        struct result ret = {};

        if ((f = fopen("11.in", "r")) == NULL) {
                err = "fopen";
                goto fopen_error;
        }

        run(f, &ret);

        printf("%ld\n%ld\n", ret.a, ret.b);

        fclose(f);
fopen_error:
        if (err)
                perror(err);
        return err ? EXIT_FAILURE : EXIT_SUCCESS;
}
