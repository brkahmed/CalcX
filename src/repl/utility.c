#include "replxx.h"

#include <ctype.h>
#include <string.h>

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
            const char *name = strndup(input + start, i - start);
            TableEntry *e    = table_lookup(&ctx->table, name);
            ReplxxColor color;
            if (!e)
                color = REPLXX_COLOR_RED;
            else if (e->type == ENTRY_TYPE_NUMBER)
                color = REPLXX_COLOR_YELLOW;
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