#define _GNU_SOURCE // required to use getline function

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "eval.h"

int main(int argc, char const *argv[]) {
    if (argc > 2) {
        ERROR("Usage %s [expression]", argv[0]);
        return -1;
    } if (argc == 2) {
        TRY(eval_expr_err_handler)
            printf("%f\n", eval_expr(argv[1]));
        EXCEPT()
            return -1;
        return 0;
    }

    char *buff = NULL;
    size_t buff_size = 0;
    ssize_t buff_len = 0;

    while (true) {
        printf(">>> ");
        buff_len = getline(&buff, &buff_size, stdin);
        if (buff_len == -1)
            break;
        TRY(eval_expr_err_handler) {
            printf("%f\n", eval_expr(buff));
        }
    }
    printf("\n");

    free(buff);
    return 0;
}
