#include <stdio.h>
#include "colors.h"

char *red = "[0;31m";
char *bold_red = "[1;31m";
char *green = "[0;32m";
char *bold_green = "[1;32m";
char *yellow = "[0;33m";
char *bold_yellow = "[1;33m";
char *blue = "[0;34m";
char *bold_blue = "[1;34m";
char *magenta = "[0;35m";
char *bold_magenta = "[1;35m";
char *cyan = "[0;36m";
char *bold_cyan = "[1;36m";
char *reset = "[0m";

int color_on = 0;

void color(const char *color) {
    if (color_on)
        printf("\033%s", color);
}
