#include "Control.h"
#include <iostream>

namespace core::control
{
	auto test_intersect_with_triangle(const Ray& ray, const vec3& p0, const vec3& p1, const vec3& p2) -> bool
	{
		//O + td = (1-b1-b2)p0 + b1*p1 + b2*p2
		auto e1 = p1 - p0;
		auto e2 = p2 - p0;
		auto s = ray.origin - p0;
		auto s1 = glm::cross(ray.direction, e2);
		auto s2 = glm::cross(s, e1);
		auto result = vec3{ glm::dot(s2, e2), glm::dot(s1, s), glm::dot(s2, ray.direction) } / (glm::dot(s1, e1));
		// result {t, b1, b2}
		auto& t = result.x;
		auto& b1 = result.y;
		auto& b2 = result.z;
		if (result.t > 0 && b1 > 0 && b2 > 0 && (1 - b1 - b2) > 0) {
			return true;
		}
		else {
			return false;
		}
	}

	auto Bounding_Box::intersect(const Ray& ray) -> bool
	{
		// o + td  = min.x
		// tx_min = (min.x - o.x) // d.x
		// ty_min = (min.y - o.y) // d.y

		vec3 tmin, tmax;

		if (ray.direction.x == 0.0f) {
			tmin.x = FLT_MIN;
			tmax.x = FLT_MAX;
		} else {
			auto t1 = (min.x - ray.origin.x) / ray.direction.x;
			auto t2 = (max.x - ray.origin.x) / ray.direction.x;
			tmin.x = std::min(t1, t2);
			tmax.x = std::max(t1, t2);
		}

		if (ray.direction.y == 0.0f) {
			tmin.y = FLT_MIN;
			tmax.y = FLT_MAX;
		}
		else {
			auto t1 = (min.y - ray.origin.y) / ray.direction.y;
			auto t2 = (max.y - ray.origin.y) / ray.direction.y;
			tmin.y = std::min(t1, t2);
			tmax.y = std::max(t1, t2);
		}

		if (ray.direction.z == 0.0f) {
			tmin.z = FLT_MIN;
			tmax.z = FLT_MAX;
		}
		else {
			auto t1 = (min.z - ray.origin.z) / ray.direction.z;
			auto t2 = (max.z - ray.origin.z) / ray.direction.z;
			tmin.z = std::min(t1, t2);
			tmax.z = std::max(t1, t2);
		}
		
		auto t_enter = std::max(std::max(tmin.x, tmin.y), tmin.z);
		auto t_exit = std::min(std::min(tmax.x, tmax.y), tmax.z);

		if (t_enter > 0 && t_enter < t_exit) {
			return true;
		} else {
			return false;
		}

	}
}