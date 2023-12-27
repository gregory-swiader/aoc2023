#include "aoc.h"

#define NUM_RACES 4

struct data {
        int64_t times[NUM_RACES + 1], dists[NUM_RACES + 1], res[NUM_RACES + 1];
};

static int64_t binary_search(int64_t time, int64_t dist) {
        int64_t low = 0, high = time, mid, est, est2;

        while (true) {
                mid  = (low + high) / 2;
                est  = (time - mid) * mid;
                est2 = (time - (mid + 1)) * (mid + 1);
                printf("trying %ld, gives %ld\n", mid, est);
                if (est <= dist && dist < est2)
                        return mid + 1;
                if (est <= dist)
                        low = mid;
                if (dist < est)
                        high = mid;
        }
}

__attribute_maybe_unused__ static void count_ways(int64_t time, int64_t dist,
                                                  int64_t *out) {
        int64_t res = time + 1 - 2 * binary_search(time, dist);
        printf("score %ld\n", res);
        *out = res;
}

static int64_t shift(int64_t n) {
        int res = 1;
        while (n) {
                res *= 10;
                n   /= 10;
        }
        return res;
}

static int64_t coalesce(int64_t *parts) {
        int64_t res = 0;
        for (int64_t i = 0; i < NUM_RACES; i++) {
                res = res * shift(parts[i]) + parts[i];
        }
        return res;
}

char *run(FILE *f, struct result *out) {
        char       *err   = NULL;
        struct data races = {};

        fscanf(f, "Time:    %6ld %6ld %6ld %6ld\n", races.times + 0,
               races.times + 1, races.times + 2, races.times + 3);
        fscanf(f, "Distance:%6ld %6ld %6ld %6ld", races.dists + 0,
               races.dists + 1, races.dists + 2, races.dists + 3);

        races.times[NUM_RACES] = coalesce(races.times);
        races.dists[NUM_RACES] = coalesce(races.dists);
        // printf("Race: %ld, %ld\n", races.times[NUM_RACES],
        //        races.dists[NUM_RACES]);

        for (int64_t i = 0; i <= NUM_RACES; i++) {
                printf("Race: %ld, %ld\n", races.times[i], races.dists[i]);
                count_ways(races.times[i], races.dists[i], &races.res[i]);
        }

        out->a = 1;
        for (int64_t i = 0; i < NUM_RACES; i++)
                out->a *= races.res[i];
        out->b = races.res[NUM_RACES];

        return err;
}
