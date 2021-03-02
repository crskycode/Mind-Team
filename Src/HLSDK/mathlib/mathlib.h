#ifndef MATHLIB_H
#define MATHLIB_H

#define _USE_MATH_DEFINES

#include <math.h>

inline float SimpleSpline( float value )
{
	float valueSquared = value * value;
	return (3 * valueSquared - 2 * valueSquared * value);
}

inline float RandomFloat(float min, float max)
{
	return (min + max) / 2.0f;
}

#endif