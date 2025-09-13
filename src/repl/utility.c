#include "replxx.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <eval.h>

void show_result(const char *input, replxx_hints *hints, int *context_len, ReplxxColor *color, void *_ctx) {
    EvalContext *ctx = (EvalContext *)_ctx;
    Number result    = eval(ctx, input);
    if (ctx->error_type) return;
    *color      = REPLXX_COLOR_LIGHTGRAY;
    size_t skip = *context_len + 2; // 2 is a good number <3
    char buff[EVAL_STRINGIFY_BUFFSIZE + skip];
    memset(buff, ' ', skip);
    eval_stringify(buff + skip, EVAL_STRINGIFY_BUFFSIZE, result);
    replxx_add_hint(hints, buff);
}

void complete(const char *input, replxx_completions *completions, int *context_len, void *_ctx) {
    EvalContext *ctx = (EvalContext *)_ctx;
    const char *name = input + strlen(input) - *context_len;
    TableIterator iterator;
    table_iter_init(&ctx->table, &iterator);
    for (TableEntry *e; (e = table_iter_next(&iterator));) {
        if (strncasecmp(name, e->name, *context_len) == 0) replxx_add_completion(completions, e->name);
    }
}

ReplxxActionResult close_parenthesis(int code, void *_replxx) {
    (void)code; // stop compiler warning
    Replxx *replxx = (Replxx *)_replxx;
    replxx_invoke(replxx, REPLXX_ACTION_INSERT_CHARACTER, '(');
    replxx_invoke(replxx, REPLXX_ACTION_INSERT_CHARACTER, ')');
    replxx_invoke(replxx, REPLXX_ACTION_MOVE_CURSOR_LEFT, 0);
    return REPLXX_ACTION_RESULT_CONTINUE;
}

void highlight(char const *input, ReplxxColor *colors, int size, void *_ctx) {
    EvalContext *ctx = (EvalContext *)_ctx;

    int parenthesis_depth           = 0;
    ReplxxColor parenthesis_color[] = {
        REPLXX_COLOR_BRIGHTGREEN, REPLXX_COLOR_BROWN,         REPLXX_COLOR_YELLOW,
        REPLXX_COLOR_BRIGHTRED,   REPLXX_COLOR_BRIGHTMAGENTA, REPLXX_COLOR_BRIGHTBLUE,
    };

    for (int i = 0; i < size; i++) {
        if (isdigit(input[i]) || input[i] == '.')
            colors[i] = REPLXX_COLOR_BRIGHTBLUE;
        else if (isalpha(input[i]) || input[i] == '_') {
            int start = i;
            while (isalnum(input[i]) || input[i] == '_') i++;
            char *name    = strndup(input + start, i - start);
            TableEntry *e = table_lookup(&ctx->table, name);
            free(name);
            ReplxxColor color;
            if (!e)
                color = replxx_color_underline(REPLXX_COLOR_RED);
            else if (e->type == ENTRY_TYPE_NUMBER)
                color = REPLXX_COLOR_BROWN;
            else
                color = REPLXX_COLOR_BRIGHTMAGENTA;
            while (start < i) colors[start++] = color;
        } else if (input[i] == '(')
            colors[i] = parenthesis_color[parenthesis_depth++ % (sizeof(parenthesis_color) / sizeof(ReplxxColor))];
        else if (input[i] == ')') {
            colors[i] = parenthesis_color[--parenthesis_depth % (sizeof(parenthesis_color) / sizeof(ReplxxColor))];
        } else if (isspace(input[i])) {
            colors[i] = REPLXX_COLOR_DEFAULT;
        } else {
            colors[i] = REPLXX_COLOR_RED;
        }
    }
}