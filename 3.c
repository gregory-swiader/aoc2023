#include "aoc.h"
#include <ctype.h>
#include <math.h>

#define MAX_DIM 200

struct point {
        int64_t x, y;
};

struct vector {
        int64_t size;
        struct {
                struct point gear_placement;
                int64_t      size, nums[8];
        } arr[1000];
};

struct data {
        int64_t size;
        int8_t  vis[MAX_DIM][MAX_DIM];
        struct {
                char  *str;
                size_t len;
        } strarr[MAX_DIM];
        struct vector gear_map;
};

static bool eq(struct point *p, struct point *q) {
        return p->x == q->x && p->y == q->y;
}

static void push(struct vector *v, struct point *gear, int64_t num) {
        int64_t i;
        for (i = 0; i < v->size; i++)
                if (eq(&v->arr[i].gear_placement, gear))
                        return (void)(v->arr[i].nums[v->arr[i].size++] = num);

        i                                = v->size;
        v->arr[i].gear_placement         = *gear;
        v->arr[i].nums[v->arr[i].size++] = num;
        v->size++;
}

static bool is_gear(char c) { return c == '*'; }
static bool is_symbol(char c) { return !isdigit(c) && c != '.'; }

static bool in_bounds(int64_t i, int64_t max) { return 0 <= i && i < max; }

static bool dfs_find_gear(struct data *board, int64_t start_i, int64_t start_j,
                          struct point *out) {
        bool ret = false;
        if (board->vis[start_i][start_j] != (int8_t) true)
                return false;
        board->vis[start_i][start_j] = 2;
        for (int64_t i = start_i - 1; i <= start_i + 1; i++)
                for (int64_t j = start_j - 1; j <= start_j + 1; j++)
                        if (in_bounds(i, board->size) &&
                            in_bounds(j, board->strarr[i].len) &&
                            board->vis[i][j] == (int8_t) true) {
                                if (is_gear(board->strarr[i].str[j])) {
                                        ret    = true;
                                        out->x = i;
                                        out->y = j;
                                }
                                if (isdigit(board->strarr[i].str[j]) &&
                                    dfs_find_gear(board, i, j, out))
                                        ret = true;
                        }
        return ret;
}

static void dfs_mark_vis(struct data *board, int64_t start_i, int64_t start_j) {
        if (!board->vis[start_i][start_j])
                board->vis[start_i][start_j] = true;
        for (int64_t i = start_i - 1; i <= start_i + 1; i++)
                for (int64_t j = start_j - 1; j <= start_j + 1; j++)
                        if (in_bounds(i, board->size) &&
                            in_bounds(j, board->strarr[i].len) &&
                            !board->vis[i][j] &&
                            isdigit(board->strarr[i].str[j]))
                                dfs_mark_vis(board, i, j);
}

__attribute__((unused)) static void print_vis(struct data *board) {
        for (int64_t i = 0; i < board->size; i++) {
                for (int64_t j = 0; j < board->size; j++)
                        printf("%d", board->vis[i][j]);
                printf("\n");
        }
        printf("\n");
}

static void count_results(struct data *board, struct result *out) {
        for (int64_t i = 0; i < board->size; i++) {
                for (int64_t j = 0; j < board->size; j++) {
                        if (isdigit(board->strarr[i].str[j]) &&
                            board->vis[i][j]) {
                                struct point gear = {-1, -1};
                                int64_t num  = atoi(&board->strarr[i].str[j]);
                                out->a      += num;
                                j           += (int64_t)log10(num);
                                if (dfs_find_gear(board, i, j, &gear)) {
                                        push(&board->gear_map, &gear, num);
                                }
                        }
                }
        }

        for (int64_t i = 0; i < board->gear_map.size; i++)
                if (board->gear_map.arr[i].size == 2)
                        out->b += board->gear_map.arr[i].nums[0] *
                                  board->gear_map.arr[i].nums[1];
}

char *run(FILE *f, struct result *out) {
        char       *line = NULL, *err = NULL;
        size_t      len;
        struct data board = {};
        int64_t     i, j;

        while ((getline(&line, &len, f)) != -1) {
                board.strarr[board.size].str = strdup(line);
                board.strarr[board.size].len = strlen(line) - 1;
                board.size++;
        }

        for (i = 0; i < board.size; i++)
                for (j = 0; j < board.strarr[i].len; j++)
                        if (is_symbol(board.strarr[i].str[j]))
                                dfs_mark_vis(&board, i, j);

        count_results(&board, out);

        free(line);
        for (i = 0; i < board.size; i++)
                free(board.strarr[i].str);
        return err;
}
