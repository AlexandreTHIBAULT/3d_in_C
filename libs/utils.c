#include "utils.h"

float modPI(float nb){

    while(nb>M_PI){
        nb -= M_PI * 2.f;
    }
    while(nb<=-M_PI){
        nb += M_PI * 2.f;
    }

    return nb;
}