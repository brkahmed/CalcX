#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "eval/eval.h"
#include "replxx.h"

int main(int arg, char const *argv[]) {
    if (arg > 2) {
        ERROR("Usage %s [expression]", argv[0]);
        return -1;
    }
    if (arg == 2) {
        TRY(eval_expr_err_handler)
        printf("%f\n", eval_expr(argv[1]));
        EXCEPT()
        return -1;
        return 0;
    }

    Replxx *replxx = replxx_init();
    while (true) {
        const char *input = NULL;
        do {
            input = replxx_input(replxx, ">>> ");
        } while (input == NULL && errno == EAGAIN);
        if (input == NULL) break;

        TRY(eval_expr_err_handler) printf("%f\n", eval_expr(input));
    }
    printf("\n");
    return 0;
}
