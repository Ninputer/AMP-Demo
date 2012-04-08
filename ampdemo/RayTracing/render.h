#pragma once

#include "amp.h"
#include "amp_math.h"
#include "ampvectors.h"
#include "raycommon.h"

template <typename fp_t>
class color
{
public:
	fp_t r;
	fp_t g;
	fp_t b;

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

	static color black() { return color(0.0f, 0.0f, 0.0f); }
	static color white() { return color(1.0f, 1.0f, 1.0f); }
	static color red() { return color(1.0f, 0.0f, 0.0f); }
	static color green() { return color(0.0f, 1.0f, 0.0f); }
	static color blue() { return color(0.0f, 0.0f, 1.0f); }
};

class material
{
public:
	enum material_type
	{
		material_checker,
		material_phong
	};

	template<typename fp_t> 
	color<fp_t> sample(ray<fp_t> ray, vector3<fp_t> position, vector3<fp_t> normal) const restrict(cpu, amp)
	{

	}
protected:
	explicit material(int type) restrict(cpu, amp) : type(type) {}

private:
	int type;

};

template <typename fp_t>
class checker : public material
{
public:
	checker(fp_t scale, fp_t reflectiveness) restrict(cpu, amp) : material(material_checker), scale(scale), reflectiveness(reflectiveness) {}

	color<fp_t> sample_impl(ray<fp_t> ray, vector3<fp_t> position, vector3<fp_t> normal)const restrict(cpu, amp)
	{

	}
private:
	fp_t scale;
	fp_t reflectiveness;
};


template <typename fp_t>
class sphere
{
public:
	vector3<fp_t> center;
	fp_t radius;

	explicit sphere(const vector3<fp_t>& center, fp_t radius) restrict(cpu, amp) : center(center), radius(radius) { init(); }
	sphere(const sphere& other) restrict(cpu, amp) : center(other.center), radius(other.radius) { init(); }

	intersect_result<fp_t> intersect(const ray<fp_t>& ray) const restrict(amp)
	{
		vector3<fp_t> v = ray.origin - center;
		fp_t a0 = v.sqr_length() - sqr_radius;
		fp_t d_dot_v = ray.direction.dot(v);

		if (d_dot_v <= 0 )
		{
			fp_t discr = d_dot_v * d_dot_v - a0;
			fp_t distance = -d_dot_v - math_helper<fp_t>::sqrt(discr);
			vector3<fp_t> position(ray.get_point(distance));

			if (discr >= 0)
			{
				return intersect_result<fp_t>(
					true, 
					0, 
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
void render_depth(const Concurrency::array_view<unsigned int, 2>& result)
{
	using namespace Concurrency;

	sphere<fp_t> scene(vector3<fp_t>(0, 10, -10), 10);
	perspective_camera<fp_t> camera(vector3<fp_t>(0, 10, 10), vector3<fp_t>(0, 0, -1), vector3<fp_t>(0, 1, 0), 90);

	const float max_depth = 20.0f;

	const int width = result.extent[1];
	const int height = result.extent[0];

	const int xshift = (width - 640) / 2;
	const int yshift = (height - 640) / 2;

	parallel_for_each(result.extent, [=](index<2> idx) restrict(amp)
	{
		const int x = idx[1];
		const int y = idx[0];

		fp_t sy = 1.0f - static_cast<fp_t>(y - yshift) / 640 ;
		fp_t sx = static_cast<fp_t>(x - xshift) / 640;

		ray<fp_t> ray(camera.generate_ray(sx, sy));
		intersect_result<fp_t> ir = scene.intersect(ray);

		unsigned int r = 0;
		unsigned int g = 0;
		unsigned int b = 0;

		if (ir.is_hit)
		{
			float depth = 255.0f - 255.0f * direct3d::saturate(static_cast<float>(ir.distance / max_depth));

			unsigned int intdepth = static_cast<unsigned int>(depth);
			r = intdepth;
			g = intdepth;
			b = intdepth;
		}

		result[idx] = 0xff000000 | (r << 16) | (g << 8) | b;
	});
}

template <typename fp_t>
void render_normal(const Concurrency::array_view<unsigned int, 2>& result)
{
	using namespace Concurrency;

	sphere<fp_t> scene(vector3<fp_t>(0, 10, -10), 10);
	perspective_camera<fp_t> camera(vector3<fp_t>(0, 10, 10), vector3<fp_t>(0, 0, -1), vector3<fp_t>(0, 1, 0), 90);

	const int width = result.extent[1];
	const int height = result.extent[0];

	const int xshift = (width - 640) / 2;
	const int yshift = (height - 640) / 2;

	parallel_for_each(result.extent, [=](index<2> idx) restrict(amp)
	{
		const int x = idx[1];
		const int y = idx[0];

		fp_t sy = 1.0f - static_cast<fp_t>(y - yshift) / 640 ;
		fp_t sx = static_cast<fp_t>(x - xshift) / 640;

		ray<fp_t> ray(camera.generate_ray(sx, sy));
		intersect_result<fp_t> ir = scene.intersect(ray);

		unsigned int r = 0;
		unsigned int g = 0;
		unsigned int b = 0;

		if (ir.is_hit)
		{
			r = static_cast<unsigned int>((ir.normal.x + 1) * 128);
			g = static_cast<unsigned int>((ir.normal.y + 1) * 128);
			b = static_cast<unsigned int>((ir.normal.z + 1) * 128);
		}

		result[idx] = 0xff000000 | (r << 16) | (g << 8) | b;
	});
}