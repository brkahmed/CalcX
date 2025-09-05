#include "repl.h"

#include <errno.h>
#include <stdbool.h>

#include <eval.h>
#include <replxx.h>

#include "utility.c"

void repl(void) {
    Replxx *replxx = replxx_init();
    replxx_install_window_change_handler(replxx);
    replxx_set_ignore_case(replxx, true);
    replxx_set_highlighter_callback(replxx, highlight, replxx);
    // replxx_bind_key(replxx, '(', close_parenthesis, replxx);

    EvalContext ctx;
    eval_ctx_init(&ctx);

    while (true) {
        const char *input = NULL;
        do {
            input = replxx_input(replxx, ">>> ");
        } while (input == NULL && errno == EAGAIN);
        if (input == NULL) break; // Ctrl+D

        replxx_history_add(replxx, input);

        Number result = eval(&ctx, input);
        if (ctx.error_type)
            replxx_print(replxx, "Error: %s\n", ctx.error_msg);
        else
            replxx_print(replxx, "ans: %Lf\n", result);
    }
    replxx_print(replxx, "\n");
    replxx_end(replxx);
}