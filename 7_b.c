#include "aoc.h"

#define CARDS_IN_HAND 5

#define FOR_EACH_CARD_TYPE(X)                                                  \
        X('2', TWO, 2)                                                         \
        X('3', THREE, 3)                                                       \
        X('4', FOUR, 4)                                                        \
        X('5', FIVE, 5)                                                        \
        X('6', SIX, 6)                                                         \
        X('7', SEVEN, 7)                                                       \
        X('8', EIGHT, 8)                                                       \
        X('9', NINE, 9)                                                        \
        X('T', TEN, 10)                                                        \
        X('J', JACK, 1)                                                        \
        X('Q', QUEEN, 12)                                                      \
        X('K', KING, 13)                                                       \
        X('A', ACE, 14)

#define FOR_EACH_HAND_TYPE(X)                                                  \
        X('1', HIGH_CARD, 1)                                                   \
        X('2', PAIR, 2)                                                        \
        X('3', TWO_PAIRS, 3)                                                   \
        X('4', THREE_OF_A_KIND, 4)                                             \
        X('5', FULL_HOUSE, 5)                                                  \
        X('6', FOUR_OF_A_KIND, 6)                                              \
        X('7', FIVE_OF_A_KIND, 7)

#define ENUMERATE(ID, X, VAL) X = VAL,
#define SWITCH_STR(ID, X, VAL)                                                 \
        case X: return #X;
#define SWITCH_PARSE(ID, X, VAL)                                               \
        case ID: return X;

#define MAKE_ENUM(NAME, MACRO_FOR_EACH)                                        \
        enum NAME { MACRO_FOR_EACH(ENUMERATE) };                               \
        enum NAME char_to_##NAME(int c) {                                      \
                switch (c) {                                                   \
                        MACRO_FOR_EACH(SWITCH_PARSE)                           \
                default: return -1;                                            \
                }                                                              \
        }                                                                      \
        char *NAME##_to_str(enum NAME e) {                                     \
                switch (e) {                                                   \
                        MACRO_FOR_EACH(SWITCH_STR)                             \
                default: return NULL; /* unreached */                          \
                }                                                              \
        }

MAKE_ENUM(card_rank, FOR_EACH_CARD_TYPE);
MAKE_ENUM(hand_rank, FOR_EACH_HAND_TYPE);

struct data {
        int64_t size;
        struct hand {
                enum card_rank cards[5];
                int64_t        bid;
        } hands[MAX_LEN];
};

enum hand_rank rank(struct hand *hand) {
        int8_t  card_counts[16] = {}, count_groups[6] = {};
        int32_t i;
        for (i = 0; i < CARDS_IN_HAND; i++)
                card_counts[hand->cards[i]]++;
        for (i = TWO; i <= ACE; i++)
                count_groups[card_counts[i]]++;

        if (count_groups[5 - card_counts[JACK]])
                return FIVE_OF_A_KIND;
        if (count_groups[4 - card_counts[JACK]])
                return FOUR_OF_A_KIND;

        // Now, if there is count_groups[3], there can't be any JACKs
        if (count_groups[3])
                assert(card_counts[JACK] == 0);

        if (count_groups[3] && count_groups[2])
                return FULL_HOUSE;
        if (count_groups[3])
                return THREE_OF_A_KIND;

        // If there are 3 JACKs, then they make a FOUR_OF_A_KIND with any card
        // Therefore, there are 2 or fewer JACKs.
        assert(card_counts[JACK] <= 2);
        // It might actually be 1 or fewer, but I don't have a proof for that.
        if (count_groups[2] == 2) {
                assert(card_counts[JACK] <= 1);
                return card_counts[JACK] ? FULL_HOUSE : TWO_PAIRS;
        }
        if (card_counts[JACK] == 2)
                return THREE_OF_A_KIND;

        // At this point there can be at most 1 JACK
        assert(card_counts[JACK] <= 1);
        if (count_groups[2])
                return card_counts[JACK] ? THREE_OF_A_KIND : PAIR;
        return card_counts[JACK] ? PAIR : HIGH_CARD;
}

/**
 * @brief                       Three-way compare of hands.
 *                              Mnemonic: take `?` to be one of `<`, `==`, `>`.
 *                              Whenever `(comp(e1, e2) ? 0)` evaluates to true,
 *                              `(e1 ? e2)` holds.
 * @return                      Three cases:
 *                              - left >  right -> 1
 *                              - left == right -> 0
 *                              - left <  right -> -1
 */
int64_t comp(struct hand *left, struct hand *right) {
        enum hand_rank l_rank = rank(left), r_rank = rank(right);
        if (l_rank < r_rank)
                return -1;
        if (l_rank > r_rank)
                return 1;
        // hand ranks are equal, what about cards?
        for (int i = 0; i < CARDS_IN_HAND; i++) {
                if (left->cards[i] < right->cards[i])
                        return -1;
                if (left->cards[i] > right->cards[i])
                        return 1;
        }
        // they're all equal
        return 0;
}

static void swap(struct hand *h1, struct hand *h2) {
        struct hand tmp = *h1;
        *h1             = *h2;
        *h2             = tmp;
}

static void sort_hands(struct data *cards) {
        for (int64_t i = 0; i < cards->size; i++)
                for (int64_t j = i + 1; j < cards->size; j++)
                        if (comp(&cards->hands[i], &cards->hands[j]) > 0)
                                swap(&cards->hands[i], &cards->hands[j]);
}

static void parse_line(char *line, struct hand *hand) {
        for (int64_t i = 0; i < CARDS_IN_HAND; i++)
                hand->cards[i] = char_to_card_rank(line[i]);
        hand->bid = atoi(line + 6);

        // for (int64_t i = 0; i < CARDS_IN_HAND; i++)
        //         printf("%s ", card_rank_to_str(hand->cards[i]));
        // printf("%ld\n", hand->bid);
}

char *run(FILE *f, struct result *out) {
        char        line[MAX_LEN], *err = NULL;
        struct data cards = {};
        int64_t     i     = 0;

        while (fgets(line, sizeof(line), f) != NULL)
                parse_line(line, &cards.hands[i++]);

        cards.size = i;
        sort_hands(&cards);

        for (i = 0; i < cards.size; i++)
                out->b += cards.hands[i].bid * (i + 1);

        return err;
}


__attribute__((unused)) static struct hand *get_hand(char *cards) {
        struct hand *ret = malloc(sizeof(*ret));
        for (int64_t i = 0; i < CARDS_IN_HAND; i++)
                ret->cards[i] = char_to_card_rank(cards[i]);
        return ret;
}

// __attribute__((unused)) int main(int argc, char *argv[]) {
//         // 5 J
//         assert(rank(get_hand("JJJJJ")) == FIVE_OF_A_KIND);
//         // 4 J
//         assert(rank(get_hand("2JJJJ")) == FIVE_OF_A_KIND);
//         // 3 J
//         assert(rank(get_hand("22JJJ")) == FIVE_OF_A_KIND);
//         assert(rank(get_hand("32JJJ")) == FOUR_OF_A_KIND);
//         // 2 J
//         assert(rank(get_hand("222JJ")) == FIVE_OF_A_KIND);
//         assert(rank(get_hand("322JJ")) == FOUR_OF_A_KIND);
//         assert(rank(get_hand("234JJ")) == THREE_OF_A_KIND);
//         // 1 J
//         assert(rank(get_hand("2222J")) == FIVE_OF_A_KIND);
//         assert(rank(get_hand("3222J")) == FOUR_OF_A_KIND);
//         assert(rank(get_hand("2233J")) == FULL_HOUSE);
//         assert(rank(get_hand("2234J")) == THREE_OF_A_KIND);
//         assert(rank(get_hand("2345J")) == PAIR);
//         // 0 J
//         assert(rank(get_hand("22222")) == FIVE_OF_A_KIND);
//         assert(rank(get_hand("32222")) == FOUR_OF_A_KIND);
//         assert(rank(get_hand("22333")) == FULL_HOUSE);
//         assert(rank(get_hand("22343")) == TWO_PAIRS);
//         assert(rank(get_hand("22234")) == THREE_OF_A_KIND);
//         assert(rank(get_hand("22345")) == PAIR);
//         assert(rank(get_hand("23456")) == HIGH_CARD);
//         return EXIT_SUCCESS;
// }
