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
        EvalContext ctx;
        eval_ctx_init(&ctx);
        Number result = eval(&ctx, argv[1]);
        if (ctx.error_type) {
            fprintf(stderr, "Error: %s\n", ctx.error_msg);
            return -1;
        }
        printf("%Lf\n", result);
    } else
        repl();

    return 0;
}
