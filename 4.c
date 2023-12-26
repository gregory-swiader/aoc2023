#include "aoc.h"

#define CARD_NUMBERS 10
#define OWN_NUMBERS  25
#define NR_GAMES     200

static int64_t score(int64_t *winning_nrs, int64_t *my_nrs) {
        int64_t ret = 0;
        for (int64_t i = 0; i < OWN_NUMBERS; i++) {
                for (int64_t j = 0; j < CARD_NUMBERS; j++) {
                        if (my_nrs[i] == winning_nrs[j]) {
                                ret++;
                                break;
                        }
                }
        }
        return ret;
}

static char *process_line(char *line, int64_t i, int64_t scores[NR_GAMES]) {
        int64_t card_nr, winning_nrs[CARD_NUMBERS], my_nrs[OWN_NUMBERS];

        sscanf(line, "Card %3ld:", &card_nr);
        line = strchr(line, ':') + 1;
        for (int i = 0; i < CARD_NUMBERS; i++) {
                sscanf(line, "%3ld", &winning_nrs[i]);
                line += 3;
        }
        line = strchr(line, '|') + 1;
        for (int i = 0; i < OWN_NUMBERS; i++) {
                sscanf(line, "%3ld", &my_nrs[i]);
                line += 3;
        }

        scores[i] = score(winning_nrs, my_nrs);

        return NULL;
}

char *run(FILE *f, struct result *out) {
        char    line[MAX_LEN], *err = NULL;
        int64_t scores[NR_GAMES], num_cards[NR_GAMES], nr_games, i, j;
        nr_games = 0;

        while (fgets(line, sizeof(line), f) != NULL) {
                if ((err = process_line(line, nr_games, scores)) != NULL)
                        goto error;
                nr_games++;
        }

        for (i = 0; i < nr_games; i++)
                num_cards[i] = 1;

        for (i = 0; i < nr_games; i++)
                for (j = 1; j <= scores[i] && i + j < nr_games; j++)
                        num_cards[i + j] += num_cards[i];

        for (i = 0; i < nr_games; i++) {
                out->a += scores[i] ? 1 << (scores[i] - 1) : 0;
                out->b += num_cards[i];
        }

error:
        return err;
}
