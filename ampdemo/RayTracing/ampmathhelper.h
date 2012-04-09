#pragma once

#include <amp.h>
#include <amp_math.h>
#include <math.h>

namespace gpu
{

template<typename fp_t> inline fp_t sqrt(fp_t x) restrict(cpu) { return ::sqrt(x); }
template<typename fp_t> inline fp_t rsqrt(fp_t x) restrict(cpu) { return 1 / ::sqrt(x); }
template<typename fp_t> inline fp_t tan(fp_t x) restrict(cpu) { return ::tan(x); }
template<typename fp_t> inline fp_t fabs(fp_t x) restrict(cpu) { return ::fabs(x); }
template<typename fp_t> inline fp_t floor(fp_t x) restrict(cpu) { return ::floor(x); }
template<typename fp_t> inline fp_t fmin(fp_t x, fp_t y) restrict(cpu) { return std::min(x, y); }
template<typename fp_t> inline fp_t fmax(fp_t x, fp_t y) restrict(cpu) { return std::max(x, y); }
template<typename fp_t> inline fp_t pow(fp_t x, fp_t y) restrict(cpu) { return ::pow(x, y); }

inline float sqrt(float x) restrict(amp) { return Concurrency::fast_math::sqrt(x); }
inline float rsqrt(float x) restrict(amp) { return Concurrency::fast_math::rsqrt(x); }
inline float tan(float x) restrict(amp) { return Concurrency::fast_math::tan(x); }
inline float fabs(float x) restrict(amp) { return Concurrency::fast_math::fabs(x); }
inline float floor(float x) restrict(amp) { return Concurrency::fast_math::floor(x); }
inline float fmin(float x, float y) restrict(amp) { return Concurrency::fast_math::fmin(x, y); }
inline float fmax(float x, float y) restrict(amp) { return Concurrency::fast_math::fmax(x, y); }
inline float pow(float x, float y) restrict(amp) { return Concurrency::fast_math::pow(x, y); }

inline double sqrt(double x) restrict(amp) { return Concurrency::precise_math::sqrt(x); }
inline double rsqrt(double x) restrict(amp) { return Concurrency::precise_math::rsqrt(x); }
inline double tan(double x) restrict(amp) { return Concurrency::precise_math::tan(x); }
inline double fabs(double x) restrict(amp) { return Concurrency::precise_math::fabs(x); }
inline double floor(double x) restrict(amp) { return Concurrency::precise_math::floor(x); }
inline double fmin(double x, double y) restrict(amp) { return Concurrency::precise_math::fmin(x, y); }
inline double fmax(double x, double y) restrict(amp) { return Concurrency::precise_math::fmax(x, y); }
inline double pow(double x, double y) restrict(amp) { return Concurrency::precise_math::pow(x, y); }

}