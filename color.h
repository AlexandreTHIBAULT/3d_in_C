struct color
{
    float r;
    float g;
    float b;
};

typedef struct color C_color;

const C_color C_red = {1.f, 0.f, 0.f};
const C_color C_green = {0.f, 1.f, 0.f};
const C_color C_blue = {0.f, 0.f, 1.f};
const C_color C_yellow = {1.f, 1.f, 0.f};
const C_color C_cyan = {0.f, 1.f, 1.f};
const C_color C_magenta = {1.f, 0.f, 1.f};
const C_color C_black = {0.f, 0.f, 0.f};
const C_color C_white = {1.f, 1.f, 1.f};

C_color C_darken(C_color color, float value){
    C_color color_res = color;
    color_res.r *= value;
    color_res.g *= value;
    color_res.b *= value;

    return color_res;
}