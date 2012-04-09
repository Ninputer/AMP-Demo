#pragma once

#include <amp.h>
#include <amp_math.h>
#include "raycommon.h"

class geometry
{
public:
	enum geometry_type
	{
		geometry_none,
		geometry_sphere,
		geometry_plane
	};

	template <typename fp_t>
	intersect_result<fp_t> intersect(const ray<fp_t>& ray) const restrict(amp)
	{
		switch (type)
		{
		case geometry_sphere:
			return static_cast<const sphere<fp_t>*>(this)->intersect_impl(ray);
		case geometry_plane:
			return static_cast<const plane<fp_t>*>(this)->intersect_impl(ray);
		default:
			return intersect_result<fp_t>();
		}
	}
protected:
	geometry(int type, int material) restrict(cpu, amp) : material(material), type(type) {}
	int material;
private:
	int type;
};

template <typename fp_t>
class sphere : public geometry
{
public:
	vector3<fp_t> center;
	fp_t radius;

	explicit sphere(const vector3<fp_t>& center, fp_t radius, int material) restrict(cpu, amp) : geometry(geometry_sphere, material), center(center), radius(radius) { init(); }

	intersect_result<fp_t> intersect_impl(const ray<fp_t>& ray) const restrict(amp)
	{
		vector3<fp_t> v = ray.origin - center;
		fp_t a0 = v.sqr_length() - sqr_radius;
		fp_t d_dot_v = ray.direction.dot(v);

		if (d_dot_v <= 0 )
		{
			fp_t discr = d_dot_v * d_dot_v - a0;
			fp_t distance = -d_dot_v - gpu::sqrt(discr);
			vector3<fp_t> position(ray.get_point(distance));

			if (discr >= 0)
			{
				return intersect_result<fp_t>(
					true, 
					material, 
					distance,
					position,
					(position - center).normalize()
					);

			}
		}

		return intersect_result<fp_t>();
	}

private:
	fp_t sqr_radius;
	void init()
	{
		sqr_radius = radius * radius;
	}
};

template <typename fp_t>
class plane : public geometry
{
public:
	vector3<fp_t> normal;
	fp_t d;

	explicit plane(const vector3<fp_t>& normal, fp_t d, int material) restrict(cpu, amp) : geometry(geometry_plane, material), normal(normal), d(d) { init(); }

	intersect_result<fp_t> intersect_impl(const ray<fp_t>& ray) const restrict(amp)
	{
		fp_t a = ray.direction.dot(normal);

		if (a >= 0) return intersect_result<fp_t>();

		fp_t b = normal.dot(ray.origin - position);
		fp_t distance = -b / a;

		return intersect_result<fp_t>(true, material, distance, ray.get_point(distance), normal); 
	}
private:
	vector3<fp_t> position;
	void init()
	{
		position = normal * d;
	}
};