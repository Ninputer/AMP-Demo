#include "stdafx.h"
#include "mandelbrot.h"

unsigned int set_hsb (float hue, float saturate, float bright) restrict (amp)
{   

    float red, green, blue;      
    float h = (hue * 256) / 60;  
    float p = bright * (1 - saturate);  
    float q = bright * (1 - saturate * (h - (int)h));  
    float t = bright * (1 - saturate * (1 - (h - (int)h)));  

    switch ((int)h) {  
    case 0:   
        red = bright,  green = t,  blue = p;  
        break;  
    case 1:  
        red = q,  green = bright,  blue = p;  
        break;  
    case 2:  
        red = p,  green = bright,  blue = t;  
        break;  
    case 3:  
        red = p,  green = q,  blue = bright;  
        break;  
    case 4:  
        red = t,  green = p,  blue = bright;  
        break;  
    case 5:  
    case 6:  
        red = bright,  green = p,  blue = q;  
        break;  
    }  

    unsigned int ired, igreen, iblue;
    ired = (unsigned int)(red * 255.0f);
    igreen = (unsigned int)(green * 255.0f);
    iblue = (unsigned int)(blue * 255.0f);

    return 0xff000000 | (ired << 16) | (igreen << 8) | iblue;  
}