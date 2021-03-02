/***
 * 
 * Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 * 
 * This product contains software technology licensed from Id
 * Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 * All Rights Reserved.
 * 
 * Use, distribution, and modification of this source code and/or resulting
 * object code is restricted to non-commercial enhancements to products from
 * Valve LLC. All other use, distribution, or modification is prohibited
 * without written permission from Valve LLC.
 * 
***/

#ifndef UTIL_VECTOR_H
#define UTIL_VECTOR_H

#include <mathlib.h>


#pragma pack(1)

//----------------------------------------------------------------
// Purpose : 
//----------------------------------------------------------------
class Vector
{
public:
	inline Vector(void)
	{
		x = 0;
		y = 0;
		z = 0;
	}
	inline Vector(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}
	inline Vector(const Vector &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}
	inline Vector(const float *v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
	}

	inline Vector operator-(void)
	{
		return Vector(-x, -y, -z);
	}

	inline bool operator==(const Vector &v)
	{
		return (x == v.x && y == v.y && z == v.z);
	}
	inline bool operator!=(const Vector &v)
	{
		return (x != v.x || y != v.y || z != v.z);
	}

	inline Vector operator+(const Vector &v) const
	{
		return Vector(x + v.x, y + v.y, z + v.z);
	}
	inline Vector operator-(const Vector &v) const
	{
		return Vector(x - v.x, y - v.y, z - v.z);
	}
	inline Vector operator*(float f) const
	{
		return Vector(x * f, y * f, z * f);
	}
	inline Vector operator/(float f) const
	{
		return Vector(x / f, y / f, z / f);
	}
	inline void operator+=(const Vector &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}
	inline void operator+=(const float *v)
	{
		x += v[0];
		y += v[1];
		z += v[2];
	}
	inline void operator-=(const Vector &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}
	inline void operator-=(const float *v)
	{
		x -= v[0];
		y -= v[1];
		z -= v[2];
	}
	inline void operator*=(const Vector &v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
	}
	inline void operator*=(float f)
	{
		x *= f;
		y *= f;
		z *= f;
	}
	inline void operator/=(float f)
	{
		x /= f;
		y /= f;
		z /= f;
	}

	inline void CopyToArray(float *v)
	{
		v[0] = x;
		v[1] = y;
		v[2] = z;
	}

	inline float Length(void)
	{
		return sqrt(x * x + y * y + z * z);
	}

	inline float &operator[](int n)
	{
		if (n == 0)
			return x;
		else if (n == 1)
			return y;
		else
			return z;
	}

	inline operator float *() const
	{
		return (float *)&x;
	}
	inline operator const float *() const
	{
		return (float *)&x;
	}

	inline void Inverse(void)
	{
		x = -x;
		y = -y;
		z = -z;
	}

	inline void Swap(Vector &v)
	{
		Vector tmp = v;
		v = *this;
		*this = tmp;
	}

	inline Vector Normalize(void)
	{
		float len = Length();

		if (len == 0)
		{
			return Vector(0.0f, 0.0f, 1.0f);
		}
		else
		{
			len = 1 / len;
			return Vector(x * len, y * len, z * len);
		}
	}

	inline float Length2D(void)
	{
		return sqrt(x * x + y * y);
	}

	float x, y, z;
};

#pragma pack()

inline Vector operator*(float l, const Vector &r)
{
	return r * l;
}

#endif