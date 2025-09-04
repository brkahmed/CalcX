#include "repl.h"

#include "error.h"
#include "eval.h"
#include "replxx.h"

#include <errno.h>
#include <stdbool.h>

#include "utility.c"

void repl(void) {
    Replxx *replxx = replxx_init();
    replxx_install_window_change_handler(replxx);
    replxx_set_ignore_case(replxx, true);
    replxx_set_highlighter_callback(replxx, highlight, replxx);
    // replxx_bind_key(replxx, '(', close_parenthesis, replxx);

    while (true) {
        const char *input = NULL;
        do {
            input = replxx_input(replxx, ">>> ");
        } while (input == NULL && errno == EAGAIN);
        if (input == NULL) break; // Ctrl+D

        replxx_history_add(replxx, input);

        TRY(eval_expr_err_handler)
        replxx_print(replxx, "ans: %lf\n", eval_expr(input));
    }
    printf("\n");
    replxx_end(replxx);
}