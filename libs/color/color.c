#include "color.h"

C_color C_red     = {1.f, 0.f, 0.f};
C_color C_green   = {0.f, 1.f, 0.f};
C_color C_blue    = {0.f, 0.f, 1.f};
C_color C_yellow  = {1.f, 1.f, 0.f};
C_color C_cyan    = {0.f, 1.f, 1.f};
C_color C_magenta = {1.f, 0.f, 1.f};
C_color C_black   = {0.f, 0.f, 0.f};
C_color C_white   = {1.f, 1.f, 1.f};

C_color C_darken(C_color color, float value){
    C_color color_res = color;

    color_res.r *= value;
    color_res.g *= value;
    color_res.b *= value;

    return color_res;
}

C_color C_multiply(C_color color1, C_color color2){
    C_color color_res;

    color_res.r = color1.r * color2.r;
    color_res.g = color1.g * color2.g;
    color_res.b = color1.b * color2.b;

    return color_res;
}

C_color C_add(C_color color1, C_color color2){
    C_color color_res;

    color_res.r = color1.r + color2.r;
    color_res.g = color1.g + color2.g;
    color_res.b = color1.b + color2.b;

    return color_res;
}