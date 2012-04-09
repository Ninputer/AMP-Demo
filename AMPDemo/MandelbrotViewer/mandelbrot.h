#include "amp.h"
#include "amp_math.h"

inline unsigned int set_hsb (float hue, float saturate, float bright) restrict (amp)
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

template<typename fp_t>
void generate_mandelbrot(
    Concurrency::array_view<unsigned int, 2> result,
    unsigned int max_iter,
    fp_t real_min,
    fp_t imag_min,
    fp_t real_max,
    fp_t imag_max )
{
    using namespace Concurrency;
    using namespace fast_math;

    int width = result.extent[1];
    int height = result.extent[0];

    fp_t scale_real = (real_max - real_min) / width;
    fp_t scale_imag = (imag_max - imag_min) / height;

    const fp_t zero = static_cast<fp_t>(0.0f);
    const fp_t max_c = static_cast<fp_t>(4.0f);

    parallel_for_each(result.extent, [=](index<2> i) restrict(amp)
    {
        int gx = i[1];
        int gy = i[0];

        fp_t cx = real_min + static_cast<float>(gx) * scale_real;
        fp_t cy = imag_min + static_cast<float>(height - gy) * scale_imag;

        fp_t zx = zero;
        fp_t zy = zero;

        fp_t temp;
        fp_t length_sqr;

        unsigned int count = 0;
        do
        {
            count++;

            temp = zx * zx - zy * zy + cx;
            zy = 2 * zx * zy + cy;
            zx = temp;

            length_sqr = zx * zx + zy * zy;
        }
        while((length_sqr < max_c) && (count < max_iter));

        //faster using multiplication than division
        float n = count * 0.0078125f; // n = count / 128.0f; 

        float h = 1.0f - 2.0f * fabs(0.5f - n + floor(n));

        //turn points at maximum iteration to black
        float bfactor = direct3d::clamp((float)(max_iter - count), 0.0f, 1.0f);

        result[i] = set_hsb(h, 0.7f, (1.0f - h * h * 0.83f) * bfactor);
    });
}