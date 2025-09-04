#include "replxx.h"

#include <ctype.h>

ReplxxActionResult close_parenthesis(int code, void *userData) {
    Replxx *replxx = (Replxx *)userData;
    replxx_print(replxx, "%c\n", code);
    // replxx_print(replxx, "(");
    // replxx_emulate_key_press(replxx, ')');
    // replxx_emulate_key_press(replxx, REPLXX_KEY_LEFT);
    return REPLXX_ACTION_RESULT_CONTINUE;
}

void highlight(char const *input, ReplxxColor *colors, int size, void *_) {

    int parenthesis_depth           = 0;
    ReplxxColor parenthesis_color[] = {
        REPLXX_COLOR_BRIGHTGREEN, REPLXX_COLOR_BROWN,         REPLXX_COLOR_YELLOW,
        REPLXX_COLOR_BRIGHTRED,   REPLXX_COLOR_BRIGHTMAGENTA, REPLXX_COLOR_BRIGHTBLUE,
    };

    for (int i = 0; i < size; i++) {
        if (isdigit(input[i]) || input[i] == '.')
            colors[i] = REPLXX_COLOR_BRIGHTBLUE;
        else if (isalpha(input[i]) || input[i] == '_')
            colors[i] = REPLXX_COLOR_BRIGHTMAGENTA;
        else if (input[i] == '(')
            colors[i] = parenthesis_color[parenthesis_depth++ % (sizeof(parenthesis_color) / sizeof(ReplxxColor))];
        else if (input[i] == ')') {
            colors[i] = parenthesis_color[--parenthesis_depth % (sizeof(parenthesis_color) / sizeof(ReplxxColor))];
        } else if (input[i] == ' ') {
            colors[i] = REPLXX_COLOR_DEFAULT;
        } else {
            colors[i] = REPLXX_COLOR_RED;
        }
    }
}