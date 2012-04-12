#pragma once

#include "ampvectors.h"

template <typename fp_t>
class intersect_result
{
public:
	vector3<fp_t> position;
	vector3<fp_t> normal;
	fp_t distance;
	int material;
	bool is_hit;

	intersect_result() restrict(cpu, amp) : is_hit(false), material(0), distance(0.0f) {}
	intersect_result(const intersect_result& other) restrict(cpu, amp) 
		: is_hit(other.is_hit), material(other.material), distance(other.distance), position(other.position), normal(other.normal) {}
	explicit intersect_result(bool is_hit, int material, fp_t distance, const vector3<fp_t>& position, const vector3<fp_t>& normal) restrict(cpu, amp) 
		: is_hit(is_hit), material(material), distance(distance), position(position), normal(normal) {}
};


template <typename fp_t>
class ray
{
public:
	vector3<fp_t> origin;
	vector3<fp_t> direction;

	explicit ray(const vector3<fp_t>& origin, const vector3<fp_t>& direction) restrict(cpu, amp) : origin(origin), direction(direction) {}
	ray(const ray& other) restrict(cpu, amp) : origin(other.origin), direction(other.direction) {}

	vector3<fp_t> get_point(fp_t t) const restrict(cpu, amp)
	{
		return origin + (direction * t);
	}
};

template <typename fp_t>
class perspective_camera
{
public:
	vector3<fp_t> eye;
	vector3<fp_t> front;
	vector3<fp_t> right;
	vector3<fp_t> up;
	fp_t fov_scale;

	explicit perspective_camera(vector3<fp_t> eye, vector3<fp_t> front, vector3<fp_t> ref_up, float fov) restrict(cpu, amp) 
		: eye(eye), front(front)
	{
		right = front.cross(ref_up);
		up = right.cross(front);
		fov_scale = gpu::tan(fov * 0.5f * 3.1415926f / 180.0f) * 2.0f;
	}

	ray<fp_t> generate_ray(fp_t x, fp_t y) const restrict(cpu, amp)
	{
		vector3<fp_t> r(right * ((x - 0.5f) * fov_scale));
		vector3<fp_t> u(up * ((y - 0.5f) * fov_scale));

		return ray<fp_t>(eye, (front + r + u).normalize());
	}
};

template <typename fp_t>
class color
{
public:
	fp_t r;
	fp_t g;
	fp_t b;

	color() restrict(cpu, amp) : r(0.0f), g(0.0f), b(0.0f) {}
	explicit color(fp_t r, fp_t g, fp_t b) restrict(cpu, amp) : r(r), g(g), b(b) {}
	color(const color& other) restrict(cpu, amp) : r(other.r), g(other.g), b(other.b) {}

	color operator+(const color& c) const restrict(cpu, amp)
	{
		return color(r + c.r, g + c.g, b + c.b);
	}

	color operator*(fp_t s) const restrict(cpu, amp)
	{
		return color(r * s, g * s, b * s);
	}

	color operator*(const color& c) const restrict(cpu, amp)
	{
		return color(r * c.r, g * c.g, b * c.b);
	}

	static color black() restrict(cpu, amp) { return color(0.0f, 0.0f, 0.0f); }
	static color white() restrict(cpu, amp) { return color(1.0f, 1.0f, 1.0f); }
	static color red() restrict(cpu, amp) { return color(1.0f, 0.0f, 0.0f); }
	static color green() restrict(cpu, amp) { return color(0.0f, 1.0f, 0.0f); }
	static color blue() restrict(cpu, amp) { return color(0.0f, 0.0f, 1.0f); }
};

