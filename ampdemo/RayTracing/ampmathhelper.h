#pragma once

#include <amp.h>
#include <amp_math.h>
#include <math.h>

template<typename fp_t>
class math_helper_cpu
{
public:
	static fp_t sqrt(fp_t x) restrict(cpu) { return ::sqrt(x); }
	static fp_t rsqrt(fp_t x) restrict(cpu) { return 1 / ::sqrt(x); }
	static fp_t tan(fp_t x) restrict(cpu) { return ::tan(x); }
	static fp_t fabs(fp_t x) restrict(cpu) { return ::fabs(x); }
	static fp_t floor(fp_t x) restrict(cpu) { return ::floor(x); }
	static fp_t fmin(fp_t x, fp_t y) restrict(cpu) { return std::min(x, y); }
	static fp_t fmax(fp_t x, fp_t y) restrict(cpu) { return std::max(x, y); }
	static fp_t pow(fp_t x, fp_t y) restrict(cpu) { return ::pow(x, y); }
};

template<typename fp_t>
class math_helper;

template<>
class math_helper<float> : public math_helper_cpu<float>
{
public:
	using math_helper_cpu<float>::sqrt;
	using math_helper_cpu<float>::rsqrt;
	using math_helper_cpu<float>::tan;
	using math_helper_cpu<float>::fabs;
	using math_helper_cpu<float>::floor;
	using math_helper_cpu<float>::fmin;
	using math_helper_cpu<float>::fmax;
	using math_helper_cpu<float>::pow;

	static float sqrt(float x) restrict(amp) { return Concurrency::fast_math::sqrt(x); }
	static float rsqrt(float x) restrict(amp) { return Concurrency::fast_math::rsqrt(x); }
	static float tan(float x) restrict(amp) { return Concurrency::fast_math::tan(x); }
	static float fabs(float x) restrict(amp) { return Concurrency::fast_math::fabs(x); }
	static float floor(float x) restrict(amp) { return Concurrency::fast_math::floor(x); }
	static float fmin(float x, float y) restrict(amp) { return Concurrency::fast_math::fmin(x, y); }
	static float fmax(float x, float y) restrict(amp) { return Concurrency::fast_math::fmax(x, y); }
	static float pow(float x, float y) restrict(amp) { return Concurrency::fast_math::pow(x, y); }
};

template<>
class math_helper<double> : public math_helper_cpu<double>
{
public:
	using math_helper_cpu<double>::sqrt;
	using math_helper_cpu<double>::rsqrt;
	using math_helper_cpu<double>::tan;
	using math_helper_cpu<double>::fabs;
	using math_helper_cpu<double>::floor;
	using math_helper_cpu<double>::fmin;
	using math_helper_cpu<double>::fmax;
	using math_helper_cpu<double>::pow;

	static double sqrt(double x) restrict(amp) { return Concurrency::precise_math::sqrt(x); }
	static double rsqrt(double x) restrict(amp) { return Concurrency::precise_math::rsqrt(x); }
	static double tan(double x) restrict(amp) { return Concurrency::precise_math::tan(x); }
	static double fabs(double x) restrict(amp) { return Concurrency::precise_math::fabs(x); }
	static double floor(double x) restrict(amp) { return Concurrency::precise_math::floor(x); }
	static double fmin(double x, double y) restrict(amp) { return Concurrency::precise_math::fmin(x, y); }
	static double fmax(double x, double y) restrict(amp) { return Concurrency::precise_math::fmax(x, y); }
	static double pow(double x, double y) restrict(amp) { return Concurrency::precise_math::pow(x, y); }
};