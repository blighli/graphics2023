#pragma once
#include "Vector.h"
namespace core::ilght
{
	using namespace core::math;
	struct Point_Light
	{
		Vector3 direction;
		Vector3 color;
		Vector3 position;
		float intensity;
	};
	struct Direct_Light
	{
		Vector3 direction;
		Vector3 color;
		float intensity;
	};
}