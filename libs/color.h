/*****************/
/* --- TYPES --- */
/*****************/
#ifndef LIB_COLOR
    #define LIB_COLOR

    typedef struct color
    {
        float r;
        float g;
        float b;
    } C_color;

    /*********************/
    /* --- VARIABLES --- */
    /*********************/

    extern C_color C_red     ;
    extern C_color C_green   ;
    extern C_color C_blue    ;
    extern C_color C_yellow  ;
    extern C_color C_cyan    ;
    extern C_color C_magenta ;
    extern C_color C_black   ;
    extern C_color C_white   ;

    /*********************/
    /* --- FUNCTIONS --- */
    /*********************/

    C_color C_darken(C_color color, float value);
    C_color C_multiply(C_color color1, C_color color2);
    C_color C_add(C_color color1, C_color color2);

#endif