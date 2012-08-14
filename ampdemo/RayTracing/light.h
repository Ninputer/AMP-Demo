#pragma once

#include "geometry.h"

template<typename fp_t>
class light_sample
{
public:
	vector3<fp_t> light_vec;
	color<fp_t> energy;

	light_sample() restrict(cpu, amp) {}
	explicit light_sample(const vector3<fp_t>& l, const color<fp_t>& el) restrict(cpu, amp) : light_vec(l), energy(el) {}
	light_sample(const light_sample& other) restrict(cpu, amp) : light_vec(other.light_vec), energy(other.energy) {}
};

template<typename fp_t>
class light
{
public:
	enum light_type
	{
		light_point
	};

	light_sample<fp_t> sample(const scene_storage<fp_t>& scene, const vector3<fp_t>& pos) const restrict(cpu, amp)
	{
		return static_cast<const point_light<fp_t>*>(this)->sample(scene, pos);
	}

protected:
	explicit light(int type) { }
};

template<typename fp_t>
class point_light : public light<fp_t>
{
public:
	color<fp_t> intensity;
	vector3<fp_t> position;

	explicit point_light(const color<fp_t>& intensity, const vector3<fp_t>& position) restrict(cpu, amp) : light(light_point), intensity(intensity), position(position) {}

	light_sample<fp_t> sample(const scene_storage<fp_t>& scene, const vector3<fp_t>& pos) const restrict(cpu, amp)
	{
		vector3<fp_t> delta(position - pos);
		fp_t rr = delta.sqr_length();
		fp_t r = gpu::sqrt(rr);

		vector3<fp_t> l = delta / r;

		ray<fp_t> shadow_ray(pos, l);
		intersect_result<fp_t> shadow_result(scene.intersect(shadow_ray));

		if (shadow_result.is_hit && shadow_result.distance <= r)
			return light_sample<fp_t>();

		fp_t attenuation = 1.0f / rr;

		return light_sample<fp_t>(l, intensity * attenuation);
	}
};

template<typename fp_t>
class light_storage
{
public:
	enum
	{
		light_max_size = sizeof(point_light<fp_t>),
		light_count = 1
	};

	light_storage() restrict(cpu)
	{
		new(lights + 0) point_light<fp_t>(color<fp_t>::white() * 1000.0f, vector3<fp_t>(20, 30, 10));
	}

	light_sample<fp_t> sample(int light_id, const scene_storage<fp_t>& scene, const vector3<fp_t>& pos) const restrict(cpu, amp)
	{
		const light_value* l = &lights[light_id];

		const light<fp_t>* p = reinterpret_cast<const light<fp_t>*>(l);
		return p->sample(scene, pos);
	}


private:
	struct light_value
	{
		fp_t values[light_max_size];
	} lights[light_count];
};