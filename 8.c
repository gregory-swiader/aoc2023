#include "aoc.h"
#include <ctype.h>
#include <stdint.h>

#define ID_LEN      3
#define ALPH_LEN    26

#define ID_OFF      0
#define LEFT_OFF    7
#define RIGHT_OFF   12

#define NR_STARTS_B 6


struct cycle_info {
        int64_t count;
        struct {
                int64_t start, again;
                char    start_label[ID_LEN + 1], end_label[ID_LEN + 1];
        } cycle[NR_STARTS_B];
};

struct node {
        char left[ID_LEN], right[ID_LEN];
};

struct node map[ALPH_LEN][ALPH_LEN][ALPH_LEN] = {};

static int8_t idx(char c) { return c - (int8_t)'A'; }
static char  *left(char *id) {
        return map[idx(id[0])][idx(id[1])][idx(id[2])].left;
}
static char *right(char *id) {
        return map[idx(id[0])][idx(id[1])][idx(id[2])].right;
}

static int64_t gcd(int64_t a, int64_t b) { return b == 0 ? a : gcd(b, a % b); }
static int64_t lcm(int64_t a, int64_t b) { return a * (b / gcd(a, b)); }
static int64_t fold_lcm(int64_t *arr, int64_t len) {
        int64_t res, i;
        if (len <= 0)
                return 0;
        if (len == 1)
                return arr[0];
        res = lcm(arr[0], arr[1]);
        for (i = 2; i < len; i++)
                res = lcm(res, arr[i]);
        return res;
}

static char *parse_line(char *line, struct cycle_info *out) {
        struct node *node;
        if (!(isupper(line[0]) && isupper(line[1]) && isupper(line[2])))
                return "Unknown label";
        node = &map[idx(line[0])][idx(line[1])][idx(line[2])];
        if ((line + ID_OFF)[ID_LEN - 1] == 'A')
                memcpy(out->cycle[out->count++].start_label, line + ID_OFF,
                       ID_LEN);
        memcpy(node->left, line + LEFT_OFF, ID_LEN);
        memcpy(node->right, line + RIGHT_OFF, ID_LEN);
        return NULL;
}

static char *walk(char *start, char *end, char *moves, struct result *out) {
        int64_t res = 0, nr_moves = strlen(moves);
        char    node[ID_LEN];
        memcpy(node, start, ID_LEN);

        while (strncmp(node, end, ID_LEN) != 0) {
                switch (moves[res % nr_moves]) {
                case 'L': memcpy(node, left(node), ID_LEN); break;
                case 'R': memcpy(node, right(node), ID_LEN); break;
                default: return "Wrong move!";
                }
                res++;
        }
        out->a = res;
        return NULL;
}

static char *detect_cycles(struct cycle_info *out, char *moves) {
        int64_t nr_moves = strlen(moves), cycles_found = 0;
        char    nodes[NR_STARTS_B][ID_LEN] = {};
        int64_t i, iter;

        for (i = 0; i < out->count; i++)
                memcpy(nodes[i], out->cycle[i].start_label, ID_LEN);

        iter = 0;
        while (cycles_found < out->count) {
                switch (moves[iter % nr_moves]) {
                case 'L':
                        for (i = 0; i < out->count; i++)
                                memcpy(nodes[i], left(nodes[i]), ID_LEN);
                        break;
                case 'R':
                        for (i = 0; i < out->count; i++)
                                memcpy(nodes[i], right(nodes[i]), ID_LEN);
                        break;
                default: return "Wrong move!";
                }

                iter++;
                for (i = 0; i < out->count; i++) {
                        /* I don't like this because it assumes that in each
                         * chain, there's exactly one node ending with 'Z'
                         */
                        if (nodes[i][ID_LEN - 1] == 'Z') {
                                if (!out->cycle[i].start) {
                                        out->cycle[i].start = iter;
                                        memcpy(out->cycle[i].end_label,
                                               nodes[i], ID_LEN);
                                } else if (!out->cycle[i].again) {
                                        out->cycle[i].again =
                                                iter - out->cycle[i].start;
                                        cycles_found++;
                                }
                        }
                }
        }
        return NULL;
}

static void print_cycles(struct cycle_info *info) {
        for (int64_t i = 0; i < info->count; i++)
                printf("Path %ld: %s -> %s in %ld steps, again in %ld\n", i,
                       info->cycle[i].start_label, info->cycle[i].end_label,
                       info->cycle[i].start, info->cycle[i].again);
}

static char *find_intersection(struct cycle_info *info, struct result *out) {
        int64_t founds[NR_STARTS_B] = {};
        for (size_t i = 0; i < info->count; i++)
                founds[i] = info->cycle[i].start;
        out->b = fold_lcm(founds, info->count);
        return NULL;
}

char *run(FILE *f, struct result *out) {
        char             *line = NULL, *err = NULL;
        size_t            len = 0;
        char             *moves;
        struct cycle_info starts = {};

        if (getline(&line, &len, f) == -1) {
                err = "Couldn't read the first line\n";
                goto first_line_error;
        }
        moves = line;
        line  = NULL;
        fseek(f, 1, SEEK_CUR);

        *strchr(moves, '\n') = '\0';
        assert(strchr(moves, '\n') == NULL);

        while (getline(&line, &len, f) != -1)
                if ((err = parse_line(line, &starts)) != NULL)
                        goto parse_error;

        if ((err = walk("AAA", "ZZZ", moves, out)) != NULL)
                goto error;
        if ((err = detect_cycles(&starts, moves)) != NULL)
                goto error;
        print_cycles(&starts);
        err = find_intersection(&starts, out);

error:
parse_error:
        free(moves);
first_line_error:
        free(line);
        return err;
}
