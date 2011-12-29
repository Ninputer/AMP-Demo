#include "stdafx.h"
#include "mandelbrot.h"

using namespace Concurrency;

unsigned int set_hsb (float hue, float saturate, float bright) restrict (direct3d)
{   
    // when I wrote this, openGL only liked colors specified by RGB values. The   
    // mandelbrot routine generated an HSB color, so I wrote this routine to do   
    // the conversion. It sure isn't perfect, but it does a respectable job.   
    //   
    // I expect that part of this work (but the final openGL call) could be   
    // pushed back with the mandelbrot color generator for more speedup.   
    //   
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

void generate_mandelbrot(
	array_view<unsigned int, 2> result,
	unsigned int max_iter,
    fp_t real_min,
    fp_t imag_min,
    fp_t real_max,
    fp_t imag_max )
{
	int width = result.extent.get_x();
	int height = result.extent.get_y();

	fp_t scale_real = (real_max - real_min) / width;
	fp_t scale_imag = (imag_max - imag_min) / height;

	parallel_for_each(result.grid, [=](index<2> i) restrict(direct3d)
	{
		int gx = i.get_x();
		int gy = i.get_y();		

		fp_t cx = real_min + gx * scale_real;
		fp_t cy = imag_min + (height - gy) * scale_imag;

		fp_t zx = _F(0.0);
		fp_t zy = _F(0.0);

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
		while((length_sqr < _F(4.0)) && (count < max_iter));
    
		float normalc = count / 512.0f;
		float h = sqrt(1.0f - 1.0f / (normalc + 1.0f));
		
		//float h = sqrt(fabs(sin(count * 0.003067962f)));;
		//float h = (float)count / 1024; //* 0.001953125f;
		//h = 1.8f * fabs(0.5f - h + floor(h)) + 0.1f;
		
		float bfactor = count >= max_iter ? 0.0f : 1.0f;

		result[i] = set_hsb(h, 0.7f, (1.0f - h * h * 0.833333f) * bfactor);
	});
}