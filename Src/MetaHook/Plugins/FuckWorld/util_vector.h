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

#define _USE_MATH_DEFINES
#include <math.h>

// vector
typedef float vec2_t[2];
typedef float vec3_t[3];
typedef float vec4_t[4];

#pragma pack(1)

class Vector2D
{
public:
	inline Vector2D(void)
	{
		x = 0;
		y = 0;
	}
	inline Vector2D(float _x, float _y)
	{
		x = _x;
		y = _y;
	}

	inline Vector2D operator+(const Vector2D &v)
	{
		return Vector2D(x + v.x, y + v.y);
	}
	inline Vector2D operator-(const Vector2D &v)
	{
		return Vector2D(x - v.x, y - v.y);
	}

	inline Vector2D operator*(float f)
	{
		return Vector2D(x * f, y * f);
	}
	inline Vector2D operator/(float f)
	{
		return Vector2D(x / f, y / f);
	}

	inline float Length(void)
	{
		return sqrt(x * x + y * y);
	}

	inline Vector2D Normalize(void)
	{
		float len = Length();

		if (len == 0.0f)
		{
			return Vector2D(0.0f, 0.0f);
		}
		else
		{
			len = 1 / len;
			return Vector2D(x * len, y * len);
		}
	}

	float x, y;
};

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

	inline Vector2D Make2D(void)
	{
		return Vector2D(x, y);
	}
	inline float Length2D(void)
	{
		return sqrt(x * x + y * y);
	}

	float x, y, z;
};

#pragma pack()

inline void VectorCopy(const vec3_t in, vec3_t out)
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

inline void VectorSubtract(const vec3_t in1, const vec3_t in2, vec3_t out)
{
	out[0] = in1[0] - in2[0];
	out[1] = in1[1] - in2[1];
	out[2] = in1[2] - in2[2];
}

inline void VectorAdd(const vec3_t in1, const vec3_t in2, vec3_t out)
{
	out[0] = in1[0] + in2[0];
	out[1] = in1[1] + in2[1];
	out[2] = in1[2] + in2[2];
}

inline void VectorScale(const vec3_t in, float scale, vec3_t out)
{
	out[0] = in[0] * scale;
	out[1] = in[1] * scale;
	out[2] = in[2] * scale;
}

inline float DotProduct(const vec3_t in1, const vec3_t in2)
{
	return in1[0]*in2[0] + in1[1]*in2[1] + in1[2]*in2[2];
}

inline void CrossProduct(const vec3_t in1, const vec3_t in2, vec3_t out)
{
	out[0] = in1[1]*in2[2] - in1[2]*in2[1];
	out[1] = in1[2]*in2[0] - in1[0]*in2[2];
	out[2] = in1[0]*in2[1] - in1[1]*in2[0];
}

inline float Length(const vec3_t v)
{
	return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

#endif