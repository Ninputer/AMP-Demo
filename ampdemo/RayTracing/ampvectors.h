#pragma once

#include "ampmathhelper.h"

template<typename fp_t>
class vector3
{
public:
	fp_t x;
	fp_t y;
	fp_t z;

	explicit vector3(fp_t x, fp_t y, fp_t z) restrict(cpu, amp) : x(x), y(y), z(z) {}
	vector3() restrict(cpu, amp) : x(0.0f), y(0.0f), z(0.0f) {}
	vector3(const vector3& other) restrict(cpu, amp) : x(other.x), y(other.y), z(other.z) {}

	fp_t sqr_length() const restrict(cpu, amp)
	{
		return x * x + y * y + z * z;
	}

	fp_t length() const restrict(cpu, amp)
	{
		return gpu::sqrt(sqr_length());
	}

	vector3 normalize() const restrict(cpu, amp)
	{
		fp_t inv = gpu::rsqrt(sqr_length());

		return vector3(inv * x, inv * y, inv * z);
	}

	vector3 negate() const restrict(cpu, amp)
	{
		return vector3(-x, -y, -z);
	}

	vector3 operator+(const vector3& v2) const restrict(cpu, amp)
	{
		return vector3(x + v2.x, y + v2.y, z + v2.z);
	}

	vector3 operator-(const vector3& v2) const restrict(cpu, amp)
	{
		return vector3(x - v2.x, y - v2.y, z - v2.z);
	}

	vector3 operator*(fp_t f) const restrict(cpu, amp)
	{
		return vector3(x * f, y * f, z * f);
	}

	vector3 operator/(fp_t f) const restrict(cpu, amp)
	{
		return vector3(x / f, y / f, z / f);
	}

	fp_t dot(const vector3& v2) const restrict(cpu, amp)
	{
		return x * v2.x + y * v2.y + z * v2.z;
	}

	vector3 cross(const vector3& v2) const restrict(cpu, amp)
	{
		return vector3(-z * v2.y + y * v2.z, z * v2.x - x * v2.z, -y * v2.x + x * v2.y);
	}
};
