#include "repl.h"
#include "error.h"
#include "eval.h"

#include <replxx.h>

#include <errno.h>
#include <stdbool.h>

void repl(void) {
    Replxx *replxx = replxx_init();
    replxx_install_window_change_handler(replxx);
    replxx_set_ignore_case(replxx, true);
    while (true) {
        const char *input = NULL;
        do {
            input = replxx_input(replxx, ">>> ");
        } while (input == NULL && errno == EAGAIN);
        if (input == NULL) break;
        replxx_history_add(replxx, input);
        TRY(eval_expr_err_handler) printf("%f\n", eval_expr(input));
    }
    printf("\n");
    replxx_end(replxx);
}