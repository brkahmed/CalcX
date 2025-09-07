#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eval.h"
#include "repl.h"

int main(int argc, char const *argv[]) {
    if (argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        printf("Usage: %s [expressions...]\n", argv[0]);
        printf("  -h, --help                Show this help message\n");
        printf("\nExamples:\n");
        printf("  %s                        Start interactive REPL\n", argv[0]);
        printf("  %s x=2pi 'sin(x*ln 2)'    Evaluate expressions and print results\n", argv[0]);
        return 0;
    }

    EvalContext ctx;
    eval_init(&ctx);

    if (argc == 1)
        repl(&ctx);
    else
        for (int i = 1; i < argc; i++) {
            Number result = eval(&ctx, argv[i]);
            if (ctx.error_type) {
                fprintf(stderr, "Error: %s\n", ctx.error_msg);
                return -1;
            }
            puts(eval_stringify(NULL, 0, result));
        }

    eval_end(&ctx);
    return 0;
}
