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
#include <float.h>

/*
#pragma pack(push, 1)

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
	inline Vector2D(const Vector2D &v)
	{
		x = v.x;
		y = v.y;
	}
	inline Vector2D(const float *v)
	{
		x = v[0];
		y = v[1];
	}

	inline Vector2D operator-(void)
	{
		return Vector2D(-x, -y);
	}

	inline bool operator==(const Vector2D &v)
	{
		return (x == v.x && y == v.y);
	}
	inline bool operator==(const float *v)
	{
		return (x == v[0] && y == v[1]);
	}
	inline bool operator!=(const Vector2D &v)
	{
		return (x != v.x || y != v.y);
	}
	inline bool operator!=(const float *v)
	{
		return (x != v[0] || y != v[1]);
	}

	inline Vector2D operator+(const Vector2D &v) const
	{
		return Vector2D(x + v.x, y + v.y);
	}
	inline Vector2D operator+(const float *v) const
	{
		return Vector2D(x + v[0], y + v[1]);
	}
	inline Vector2D operator-(const Vector2D &v) const
	{
		return Vector2D(x - v.x, y - v.y);
	}
	inline Vector2D operator-(const float *v) const
	{
		return Vector2D(x - v[0], y - v[1]);
	}
	inline Vector2D operator*(float f) const
	{
		return Vector2D(x * f, y * f);
	}
	inline Vector2D operator/(float f) const
	{
		return Vector2D(x / f, y / f);
	}

	inline void CopyToArray(float *v)
	{
		v[0] = x;
		v[1] = y;
	}

	inline float Length(void)
	{
		return sqrt(x * x + y * y);
	}
	float LengthSquared()
	{
		return (x * x + y * y);
	}

	inline float &operator[](int n)
	{
		if (n == 0)
			return x;
		else
			return y;
	}

	inline operator float *()
	{
		return &x;
	}
	inline operator const float *() const
	{
		return &x;
	}

	inline Vector2D Normalize(void)
	{
		float len = Length();

		if (len == 0)
		{
			return Vector2D(0, 0);
		}
		else
		{
			len = 1 / len;
			return Vector2D(x * len, y * len);
		}
	}

	inline float NormalizeInPlace(void)
	{
		float flLen = Length();
		if (flLen > 0.0)
		{
			x = float(1 / flLen * x);
			y = float(1 / flLen * y);
		}
		else
		{
			x = 1.0;
			y = 0.0;
		}
		return flLen;
	}

	bool IsLengthLessThan(float length)
	{
		return (LengthSquared() < length * length);
	}
	bool IsLengthGreaterThan(float length)
	{
		return (LengthSquared() > length * length);
	}

	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance);
	}

	inline float DistTo(const Vector2D &vOther) const
	{
		Vector2D delta;
		delta = *this - vOther;
		return delta.Length();
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

	void Init(void)
	{
		x = 0;
		y = 0;
		z = 0;
	}
	void Init(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	inline Vector operator-(void) const
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
		return sqrt(x*x + y*y + z*z);
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

	inline float const &operator[](int n) const
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

		if (len > 0)
		{
			len = 1 / len;
			return Vector(this->x * len,
						  this->y * len,
						  this->z * len);
		}
		else
		{
			return Vector(0.0f,
						  0.0f,
						  1.0f);
		}
	}

	inline float NormalizeInPlace(void)
	{
		float len = Length();

		if (len > 0)
		{
			float s = 1.0f / len;
			this->x *= s;
			this->y *= s;
			this->z *= s;
		}
		else
		{
			this->x = 0;
			this->y = 0;
			this->z = 1;
		}

		return len;
	}

	inline Vector CrossProduct(const Vector& a) const
	{
		return Vector(
			this->y*a.z - this->z*a.y,
			this->z*a.x - this->x*a.z,
			this->x*a.y - this->y*a.x);
	}

	inline float LengthSquared(void) const
	{
		return this->x*this->x + this->y*this->y + this->z*this->z;
	}

	inline bool IsLengthGreaterThan(float val)
	{
		return LengthSquared() > val*val;
	}

	inline bool IsLengthLessThan(float val)
	{
		return LengthSquared() < val*val;
	}

	inline Vector2D Make2D(void)
	{
		return Vector2D(x, y);
	}
	inline float Length2D(void)
	{
		return sqrt(x * x + y * y);
	}

	Vector2D& AsVector2D()
	{
		return *(Vector2D *)this;
	}

	const Vector2D& AsVector2D() const
	{
		return *(const Vector2D *)this;
	}

	float LengthSqr() const
	{
		return x*x + y*y + z*z;
	}

	inline float DistTo(const Vector &vOther) const
	{
		Vector delta;
//!		VectorSubtract( *this, vOther, delta );
		delta = *this - vOther;
		return delta.Length();
	}

	bool IsZero( float tolerance = 0.01f ) const
	{
		return (x > -tolerance && x < tolerance &&
				y > -tolerance && y < tolerance &&
				z > -tolerance && z < tolerance);
	}

	inline float DistToSqr(const Vector &vOther) const
	{
		Vector delta;

		delta.x = x - vOther.x;
		delta.y = y - vOther.y;
		delta.z = z - vOther.z;

		return delta.LengthSqr();
	}

	inline float Dot( const Vector& vOther ) const
	{
		return DotProduct( *this, vOther );
	}


	float x, y, z;
};

#pragma pack(pop)

inline Vector operator*(float l, const Vector &r)
{
	return r * l;
}
//
//inline Vector2D operator*(float l, const Vector2D &r)
//{
//	return r * l;
//}

inline float DotProduct(const Vector &l, const Vector &r)
{
	return (l.x*r.x + l.y*r.y + l.z*r.z);
}
inline float DotProduct(const Vector2D &l, const Vector2D &r)
{
	return (l.x*r.x + l.y*r.y);
}

inline Vector CrossProduct(const Vector& a, const Vector& b)
{
	return a.CrossProduct(b);
}*/

#include "vector2d.h"

//=========================================================
// 3D Vector
//=========================================================
class Vector3D
{
public:
	// Members
	float x, y, z;

	// Construction
	Vector3D(void);
	Vector3D(float ix, float iy, float iz);
	Vector3D(const float *rgfl);
	Vector3D(const Vector2D &vOther);

	// Initialization
	void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f);

	// array access...
	float operator[](int i) const;
	float& operator[](int i);

	// Base address...
	operator float *();
	operator float *() const;

	// Cast to Vector2D...
	Vector2D& AsVector2D(void);
	const Vector2D& AsVector2D(void) const;

	Vector2D Make2D(void) const;

	// equality
	bool operator==(const Vector3D &v) const;
	bool operator!=(const Vector3D &v) const;

	// arithmetic operations
	Vector3D& operator+=(const Vector3D &v);
	Vector3D& operator+=(float fl);
	Vector3D& operator-=(const Vector3D &v);
	Vector3D& operator-=(float fl);
	Vector3D& operator*=(const Vector3D &v);
	Vector3D& operator*=(float s);
	Vector3D& operator/=(const Vector3D &v);
	Vector3D& operator/=(float s);

	// Get the vector's magnitude.
	float Length(void) const;

	// Get the vector's magnitude squared.
	float LengthSqr(void) const;

	// return true if this vector is (0,0,0) within tolerance
	bool IsZero(float tolerance = 0.01f) const;

	// identity of the vector
	Vector3D Normalize(void) const;

	float NormalizeInPlace(void);
	bool IsLengthGreaterThan(float val) const;
	bool IsLengthLessThan(float val) const;

	// Get the distance from this vector to the other one.
	float DistTo(const Vector3D &vOther) const;
	float DistToSqr(const Vector3D &vOther) const;

	// Copy
	void CopyToArray(float *rgfl) const;

	// Dot product.
	float Dot(const Vector3D &vOther) const;

	// assignment
	Vector3D& operator=(const Vector3D &vOther);

	// 2d
	float Length2D(void) const;
	float Length2DSqr(void) const;

	// arithmetic operations
	Vector3D operator-(void) const;

	Vector3D operator+(const Vector3D &v) const;
	Vector3D operator-(const Vector3D &v) const;
	Vector3D operator*(const Vector3D &v) const;
	Vector3D operator*(float fl) const;
	Vector3D operator/(const Vector3D &v) const;
	Vector3D operator/(float fl) const;
};


//-----------------------------------------------------------------------------
//
// Inlined Vector methods
//
//-----------------------------------------------------------------------------

inline Vector3D::Vector3D(void)
{
	x = y = z = 0.0f;
}

inline Vector3D::Vector3D(float ix, float iy, float iz)
{
	x = ix; y = iy; z = iz;
}

inline Vector3D::Vector3D(const float *rgfl)
{
	x = rgfl[0]; y = rgfl[1]; z = rgfl[2];
}

inline Vector3D::Vector3D(const Vector2D &vOther)
{
	x = vOther.x; y = vOther.y; z = 0.0f;
}

inline void Vector3D::Init(float ix, float iy, float iz)
{
	x = ix; y = iy; z = iz;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline float Vector3D::operator[](int i) const
{
	return ((float *)this)[i];
}

inline float& Vector3D::operator[](int i)
{
	return ((float *)this)[i];
}

//-----------------------------------------------------------------------------
// Base address
//-----------------------------------------------------------------------------
inline Vector3D::operator float *()
{
	return &x;
}

inline Vector3D::operator float *() const
{
	return (float *)&x;
}

//-----------------------------------------------------------------------------
// Cast to Vector2D...
//-----------------------------------------------------------------------------
inline const Vector2D& Vector3D::AsVector2D(void) const
{
	return *(const Vector2D *)this;
}

inline Vector2D& Vector3D::AsVector2D(void)
{
	return *(Vector2D *)this;
}

inline Vector2D Vector3D::Make2D(void) const
{
	return Vector2D(x, y);
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------
inline bool Vector3D::operator==(const Vector3D &v) const
{
	return (v.x == x) && (v.y == y) && (v.z == z);
}

inline bool Vector3D::operator!=(const Vector3D &v) const
{
	return (v.x != x) || (v.y != y) || (v.z != z);
}

//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------
inline Vector3D& Vector3D::operator+=(const Vector3D &v)
{
	x += v.x; y += v.y; z += v.z;
	return *this;
}

inline Vector3D& Vector3D::operator+=(float fl)
{
	x += fl; y += fl; z += fl;
	return *this;
}

inline Vector3D& Vector3D::operator-=(const Vector3D &v)
{
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}

inline Vector3D& Vector3D::operator-=(float fl)
{
	x -= fl; y -= fl; z -= fl;
	return *this;
}

inline Vector3D& Vector3D::operator*=(const Vector3D &v)
{
	x *= v.x; y *= v.y; z *= v.z;
	return *this;
}

inline Vector3D& Vector3D::operator*=(float s)
{
	x *= s; y *= s; z *= s;
	return *this;
}

inline Vector3D& Vector3D::operator/=(const Vector3D &v)
{
	x /= v.x; y /= v.y; z /= v.z;
	return *this;
}

inline Vector3D& Vector3D::operator/=(float s)
{
	x /= s; y /= s; z /= s;
	return *this;
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
inline float Vector3D::Length(void) const
{
	return sqrt(x*x + y*y + z*z);
}

inline float Vector3D::LengthSqr(void) const
{
	return x*x + y*y + z*z;
}

//-----------------------------------------------------------------------------
// check
//-----------------------------------------------------------------------------
inline bool Vector3D::IsZero(float tolerance) const
{
	return (x > -tolerance && x < tolerance &&
		    y > -tolerance && y < tolerance &&
		    z > -tolerance && z < tolerance);
}

//-----------------------------------------------------------------------------
// identity
//-----------------------------------------------------------------------------
inline Vector3D Vector3D::Normalize(void) const
{
	float radius = sqrt(x*x + y*y + z*z);

	// FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero.
	float iradius = 1.0f / (radius + FLT_EPSILON);

	return Vector3D(x*iradius, y*iradius, z*iradius);
}

inline float Vector3D::NormalizeInPlace(void)
{
	float radius = sqrt(x*x + y*y + z*z);

	// FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero.
	float iradius = 1.0f / (radius + FLT_EPSILON);

	x *= iradius;
	y *= iradius;
	z *= iradius;

	return radius;
}

inline bool Vector3D::IsLengthGreaterThan(float val) const
{
	return LengthSqr() > val*val;
}

inline bool Vector3D::IsLengthLessThan(float val) const
{
	return LengthSqr() < val*val;
}

inline float Vector3D::DistTo(const Vector3D &vOther) const
{
	return (*this - vOther).Length();
}

inline float Vector3D::DistToSqr(const Vector3D &vOther) const
{
	return (*this - vOther).LengthSqr();
}

//-----------------------------------------------------------------------------
// copy
//-----------------------------------------------------------------------------
inline void Vector3D::CopyToArray(float *rgfl) const
{
	rgfl[0] = x; rgfl[1] = y; rgfl[2] = z;
}

//-----------------------------------------------------------------------------
// dot product
//-----------------------------------------------------------------------------
inline float Vector3D::Dot(const Vector3D &vOther) const
{
	return x*vOther.x + y*vOther.y + z*vOther.z;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------
inline Vector3D& Vector3D::operator=(const Vector3D &vOther)
{
	x = vOther.x; y = vOther.y; z = vOther.z;
	return *this;
}

//-----------------------------------------------------------------------------
// 2D
//-----------------------------------------------------------------------------
inline float Vector3D::Length2D(void) const
{
	return sqrt(x*x + y*y);
}

inline float Vector3D::Length2DSqr(void) const
{
	return x*x + y*y;
}

//-----------------------------------------------------------------------------
// arithmetic operations
//-----------------------------------------------------------------------------
inline Vector3D Vector3D::operator-(void) const
{
	return Vector3D(-x, -y, -z);
}

inline Vector3D Vector3D::operator+(const Vector3D &v) const
{
	return Vector3D(x+v.x, y+v.y, z+v.z);
}

inline Vector3D Vector3D::operator-(const Vector3D &v) const
{
	return Vector3D(x-v.x, y-v.y, z-v.z);
}

inline Vector3D Vector3D::operator*(const Vector3D &v) const
{
	return Vector3D(x*v.x, y*v.y, z*v.z);
}

inline Vector3D Vector3D::operator*(float fl) const
{
	return Vector3D(x*fl, y*fl, z*fl);
}

inline Vector3D Vector3D::operator/(const Vector3D &v) const
{
	return Vector3D(x/v.x, y/v.y, z/v.z);
}

inline Vector3D Vector3D::operator/(float fl) const
{
	return Vector3D(x/fl, y/fl, z/fl);
}

//-----------------------------------------------------------------------------
// Vector related operations
//-----------------------------------------------------------------------------
inline void VectorClear(Vector3D &v)
{
	v.x = v.y = v.z = 0.0f;
}

inline void VectorCopy(const Vector3D &src, Vector3D &dst)
{
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

inline void VectorAdd(const Vector3D &a, const Vector3D &b, Vector3D &c)
{
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
}

inline void VectorSubtract(const Vector3D &a, const Vector3D &b, Vector3D &c)
{
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}

inline void VectorScale(const Vector3D &a, float b, Vector3D &c)
{
	c.x = a.x * b;
	c.y = a.y * b;
	c.z = a.z * b;
}

inline void VectorMA(const Vector3D &start, float scale, const Vector3D &direction, Vector3D &dest)
{
	dest.x = start.x + direction.x*scale;
	dest.y = start.y + direction.y*scale;
	dest.z = start.z + direction.z*scale;
}

inline float VectorLength(const Vector3D &v)
{
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

inline float DotProduct(const Vector3D &a, const Vector3D &b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

inline void CrossProduct(const Vector3D &a, const Vector3D &b, Vector3D &c)
{
	c.x = a.y*b.z - a.z*b.y;
	c.y = a.z*b.x - a.x*b.z;
	c.z = a.x*b.y - a.y*b.x;
}

// OPERATOR ( FLOAT * VECTOR )
inline Vector3D operator*(float fl, const Vector3D &v)
{
	return Vector3D(fl*v.x, fl*v.y, fl*v.z);
}

// 
inline Vector3D CrossProduct(const Vector3D &a, const Vector3D &b)
{
	Vector3D result;
	result.x = a.y*b.z - a.z*b.y;
	result.y = a.z*b.x - a.x*b.z;
	result.z = a.x*b.y - a.y*b.x;
	return result;
}

// 
typedef Vector3D Vector;


#endif