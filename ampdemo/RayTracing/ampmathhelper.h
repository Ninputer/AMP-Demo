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
};

template<typename fp_t>
class math_helper
{
public:
	static fp_t sqrt(fp_t x) restrict(cpu, amp);
	static fp_t rsqrt(fp_t x) restrict(cpu, amp);
	static fp_t tan(fp_t x) restrict(cpu, amp);
};

template<>
class math_helper<float> : public math_helper_cpu<float>
{
public:
	using math_helper_cpu<float>::sqrt;
	using math_helper_cpu<float>::rsqrt;
	using math_helper_cpu<float>::tan;

	static float sqrt(float x) restrict(amp) { return Concurrency::fast_math::sqrt(x); }
	static float rsqrt(float x) restrict(amp) { return Concurrency::fast_math::rsqrt(x); }
	static float tan(float x) restrict(amp) { return Concurrency::fast_math::tan(x); }
};

template<>
class math_helper<double> : public math_helper_cpu<double>
{
public:
	using math_helper_cpu<double>::sqrt;
	using math_helper_cpu<double>::rsqrt;
	using math_helper_cpu<double>::tan;

	static double sqrt(double x) restrict(amp) { return Concurrency::precise_math::sqrt(x); }
	static double rsqrt(double x) restrict(amp) { return Concurrency::precise_math::rsqrt(x); }
	static double tan(double x) restrict(amp) { return Concurrency::precise_math::tan(x); }
};