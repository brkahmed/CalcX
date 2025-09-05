#include <stdio.h>
#include <stdlib.h>

#include "eval.h"
#include "repl.h"

int main(int arg, char const *argv[]) {
    if (arg > 2) {
        fprintf(stderr, "Usage: %s [expression]", argv[0]);
        return -1;
    }
    if (arg == 2) {
        Number result = eval(argv[1]);
        if (eval_error_type) {
            fprintf(stderr, "Error: %s\n", eval_error_msg);
            return -1;
        }
        printf("%Lf\n", result);
    } else
        repl();

    return 0;
}
