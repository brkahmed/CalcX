#include "repl.h"

#include <errno.h>
#include <stdbool.h>

#include <eval.h>
#include <replxx.h>

#include "utility.c"

void repl(EvalContext *ctx) {
    Replxx *replxx = replxx_init();
    replxx_install_window_change_handler(replxx);
    replxx_set_ignore_case(replxx, true);
    replxx_set_double_tab_completion(replxx, true);
    replxx_set_highlighter_callback(replxx, highlight, ctx);
    replxx_bind_key(replxx, '(', close_parenthesis, replxx);
    replxx_set_completion_callback(replxx, complete, ctx);
    replxx_set_hint_callback(replxx, show_result, ctx);

    while (true) {
        const char *input = NULL;
        do {
            input = replxx_input(replxx, ">>> ");
        } while (input == NULL && errno == EAGAIN);
        if (input == NULL) break; // Ctrl+D

        replxx_history_add(replxx, input);

        Number result = eval(ctx, input);
        if (ctx->error_type)
            replxx_print(replxx, "Error: %s\n", ctx->error_msg);
        else
            replxx_print(replxx, "ans: %s\n", eval_stringify(NULL, 0, result));
    }
    replxx_print(replxx, "\n");
    replxx_end(replxx);
}