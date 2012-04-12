#pragma once

#include "light.h"

template <typename fp_t>
class material
{
public:
	enum material_type
	{
		material_checker,
		material_phong
	};
	fp_t reflectiveness;

	template<typename fp_t> 
	color<fp_t> sample(const ray<fp_t>& ray, const vector3<fp_t>& position, const vector3<fp_t>& normal, light_sample<fp_t> ls) const restrict(cpu, amp)
	{
		switch (type)
		{
		case material_checker:
			return static_cast<const checker<fp_t>*>(this)->sample_impl(ray, position, normal, ls);
		case material_phong:
			return static_cast<const phong<fp_t>*>(this)->sample_impl(ray, position, normal, ls);
		default:
			return color<fp_t>::black();
		}
	}
protected:
	explicit material(int type, fp_t reflectiveness) restrict(cpu, amp) : type(type), reflectiveness(reflectiveness) {}

private:
	int type;

};

template <typename fp_t>
class checker : public material<fp_t>
{
public:
	checker(fp_t scale, fp_t reflectiveness) restrict(cpu, amp) : material(material_checker, reflectiveness), scale(scale) {}

	color<fp_t> sample_impl(const ray<fp_t>& ray, const vector3<fp_t>& position, const vector3<fp_t>& normal, light_sample<fp_t> ls)const restrict(cpu, amp)
	{
		fp_t r = gpu::fabs(gpu::floor(position.x * scale) + gpu::floor(position.z * scale));

		color<fp_t> checker_color((static_cast<int>(r) % 2) < 1 ? color<fp_t>::black() : ls.energy);
		color<fp_t> basic_color;

		fp_t n_dot_l = normal.dot(ls.light_vec);

		if (n_dot_l >= 0)
			basic_color = basic_color + (checker_color * n_dot_l);

		return basic_color;
	}
private:
	fp_t scale;
};

template <typename fp_t>
class phong : public material<fp_t>
{
public:
	phong(color<fp_t> diffuse, color<fp_t> specular, fp_t shininess, fp_t reflectiveness)
		: material(material_phong, reflectiveness), diffuse(diffuse), specular(specular), shininess(shininess) {}

	color<fp_t> sample_impl(const ray<fp_t>& ray, const vector3<fp_t>& position, const vector3<fp_t>& normal, light_sample<fp_t> ls) const restrict(cpu, amp)
	{
		/*vector3<fp_t> light_dir(0.5773503f, 0.5773503f, 0.5773503f);
		color<fp_t> light_color(color<fp_t>::white());*/

		fp_t n_dot_l = normal.dot(ls.light_vec);
		vector3<fp_t> h((ls.light_vec - ray.direction).normalize());
		fp_t n_dot_h = normal.dot(h);
		
		color<fp_t> diffuse_term = diffuse * gpu::fmax(n_dot_l, 0.0f);
		color<fp_t> specular_term = specular * gpu::pow(gpu::fmax(n_dot_h, 0.0f), shininess);

		return ls.energy * (diffuse_term + specular_term);
	}

private:
	color<fp_t> diffuse;
	color<fp_t> specular;
	fp_t shininess;
};

template<typename fp_t>
class material_storage
{
public:
	enum
	{
		material_max_size = 16,
		material_count = 3
	};

	material_storage() restrict(cpu)
	{
		new(materials + 0) phong<fp_t>(color<fp_t>::red(), color<fp_t>::white(), 16.0f, 0.25f);
		new(materials + 1) phong<fp_t>(color<fp_t>::blue(), color<fp_t>::white(), 16.0f, 0.25f);
		new(materials + 2) checker<fp_t>(0.1f, 0.5f);
	}

	color<fp_t> sample(int material_id, const ray<fp_t>& ray, const vector3<fp_t>& position, const vector3<fp_t>& normal, light_sample<fp_t> ls) const restrict(cpu, amp)
	{
		const material_value* m = &materials[material_id];

		const material<fp_t>* p = reinterpret_cast<const material<fp_t>*>(m);
		return p->sample(ray, position, normal, ls);
	}

	fp_t get_reflectiveness(int material_id) const restrict(cpu, amp)
	{
		const material_value* m = &materials[material_id];

		const material<fp_t>* p = reinterpret_cast<const material<fp_t>*>(m);
		return p->reflectiveness;
	}

private:
	struct material_value
	{
		fp_t values[material_max_size];
	} materials[material_count];
};