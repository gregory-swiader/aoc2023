#include "aoc.h"

int32_t main(int32_t argc, char *argv[]) {
        FILE         *f   = stdin;
        char         *err = NULL;
        struct result ret = {};

        if (argc != 2) {
                err = "Usage: ./<day> <input>";
                goto error;
        }

        if ((f = fopen(argv[1], "r")) == NULL) {
                err = "fopen";
                goto error;
        }

        err = run(f, &ret);

        printf("%ld\n%ld\n", ret.a, ret.b);

        fclose(f);
error:
        if (err)
                perror(err);
        return err ? EXIT_FAILURE : EXIT_SUCCESS;
}
