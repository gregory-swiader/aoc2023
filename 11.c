#include "aoc.h"

struct elem {
        int64_t x, y;
};

struct vector_t {
        int64_t       size, __size;
        struct elem *arr;
};

typedef bool (*compare)(struct elem, struct elem);

static void push_back(struct vector_t *v, struct elem e) {
        if (v->__size == 0)
                v->__size = 1 << 8;
        while (v->__size <= v->size + 1)
                v->__size *= 2;
        v->arr = realloc(v->arr, sizeof(*v->arr) * v->__size);

        v->arr[v->size++] = e;
}

static void copy(struct vector_t *restrict dest, struct vector_t *restrict src) {
        memcpy(dest, src, sizeof(*dest));
        dest->arr = malloc(src->__size * sizeof(*src->arr));
        memcpy(dest->arr, src->arr, src->size * sizeof(*src->arr));
}

static void clear(struct vector_t *v) {
        if (v->arr)
                free(v->arr);
        memset(v, 0, sizeof(*v));
}

static int64_t count_where(struct vector_t *v, struct elem e, compare eq) {
        int64_t i, sum = 0;
        for (i = 0; i < v->size; i++)
                if (eq(v->arr[i], e))
                        sum++;
        return sum;
}

static bool eq_x(struct elem p, struct elem q) { return p.x == q.x; }
static bool eq_y(struct elem p, struct elem q) { return p.y == q.y; }
static bool less_x(struct elem p, struct elem q) { return p.x < q.x; }
static bool less_y(struct elem p, struct elem q) { return p.y < q.y; }

static void find_free(struct vector_t *galaxies, struct vector_t *rows_out,
                      struct vector_t *cols_out) {
        struct elem max_bounds = {};
        int64_t      i;

        for (i = 0; i < galaxies->size; i++) {
                max_bounds.x = MAX(max_bounds.x, galaxies->arr[i].x);
                max_bounds.y = MAX(max_bounds.y, galaxies->arr[i].y);
        }

        for (i = 0; i < MAX(max_bounds.x, max_bounds.y); i++) {
                if (count_where(galaxies, (struct elem){i, i}, eq_x) == 0)
                        push_back(rows_out, (struct elem){i, i});
                if (count_where(galaxies, (struct elem){i, i}, eq_y) == 0)
                        push_back(cols_out, (struct elem){i, i});
        }
}

static void expand_galaxies(struct vector_t *galaxies, struct vector_t *out,
                            int64_t rate) {
        struct vector_t free_rows = {}, free_columns = {};
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

        clear(&free_rows);
        clear(&free_columns);
}

static int64_t distance(struct elem a, struct elem b) {
        return labs(a.x - b.x) + labs(a.y - b.y);
}

static void find_distances(struct vector_t *galaxies, int64_t *out) {
        int64_t sum = 0, i, j;

        for (i = 0; i < galaxies->size; i++)
                for (j = i + 1; j < galaxies->size; j++)
                        sum += distance(galaxies->arr[i], galaxies->arr[j]);

        *out = sum;
}

static void get_input(FILE *f, struct vector_t *galaxies) {
        char   *line = NULL;
        size_t  line_len;
        int64_t i = 0, j;

        while (getline(&line, &line_len, f) != -1) {
                for (j = 0; j < strlen(line); j++)
                        if (line[j] == '#')
                                push_back(galaxies, (struct elem){i, j});
                i++;
        }
        free(line);
}

char *run(FILE *f, struct result *out) {
        char         *err      = NULL;
        struct vector_t galaxies = {}, galaxies_a, galaxies_b;

        get_input(f, &galaxies);
        expand_galaxies(&galaxies, &galaxies_a, 2);
        expand_galaxies(&galaxies, &galaxies_b, 1000000);
        find_distances(&galaxies_a, &out->a);
        find_distances(&galaxies_b, &out->b);

        clear(&galaxies);
        clear(&galaxies_a);
        clear(&galaxies_b);
        return err;
}
