
#include "amp.h"


#if defined FP64
#define _F(x) x
typedef double fp_t;
#else
#define _F(x) x##f
typedef float fp_t;
#endif

void generate_mandelbrot(
	Concurrency::array_view<unsigned int, 2> result,
    unsigned int max_iter,
    fp_t real_min,
    fp_t imag_min,
    fp_t real_max,
    fp_t imag_max );