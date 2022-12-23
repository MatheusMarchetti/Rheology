#pragma once

#include <cmath>

inline bool isEqual(double x, double y)
{
	const double epsilon = 1.e-15;

	return std::abs(x - y) <= epsilon * std::abs(x);
}