
#include "amp.h"

void generate_mandelbrot(
	Concurrency::array_view<unsigned int, 2> result,
    unsigned int max_iter,
    float real_min,
    float imag_min,
    float real_max,
    float imag_max );