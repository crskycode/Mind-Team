#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>

#pragma pack(push, 1)

class Vector3D
{
public:

	float x, y, z;

	Vector3D(void)
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	Vector3D(float px, float py, float pz = 0.0f)
	{
		this->x = px;
		this->y = py;
		this->z = pz;
	}

	Vector3D(const Vector3D &src)
	{
		this->x = src.x;
		this->y = src.y;
		this->z = src.z;
	}

	Vector3D(const float *src)
	{
		this->x = src[0];
		this->y = src[1];
		this->z = src[2];
	}

	Vector3D operator +(Vector3D &src)
	{
		return Vector3D(
			this->x + src.x,
			this->y + src.y,
			this->z + src.z);
	}

	Vector3D operator -(Vector3D &src)
	{
		return Vector3D(
			this->x - src.x,
			this->y - src.y,
			this->z - src.z);
	}

	Vector3D operator *(float s)
	{
		return Vector3D(
			this->x * s,
			this->y * s,
			this->z * s);
	}

	Vector3D operator /(float s)
	{
		return Vector3D(
			this->x / s,
			this->y / s,
			this->z / s);
	}

	Vector3D operator +=(Vector3D &src)
	{
		this->x += src.x;
		this->y += src.y;
		this->z += src.z;

		return *this;
	}

	Vector3D operator -=(Vector3D &src)
	{
		this->x -= src.x;
		this->y -= src.y;
		this->z -= src.z;

		return *this;
	}

	Vector3D operator *=(float s)
	{
		this->x *= s;
		this->y *= s;
		this->z *= s;

		return *this;
	}

	Vector3D operator /=(float s)
	{
		this->x /= s;
		this->y /= s;
		this->z /= s;

		return *this;
	}

	bool operator ==(Vector3D &src)
	{
		return (
			this->x == src.x && 
			this->y == src.y &&
			this->z == src.z);
	}

	bool operator !=(Vector3D &src)
	{
		return (
			this->x != src.x || 
			this->y != src.y || 
			this->z != src.z);
	}

	float &operator [](unsigned int i)
	{
		return (&this->x)[i];
	}

	operator float *()
	{
		return &this->x;
	}

	float Length(void)
	{
		return sqrtf(
			this->x * this->x + 
			this->y * this->y + 
			this->z * this->z);
	}

	Vector3D Normalize(void)
	{
		float length = Length();

		this->x /= length;
		this->y /= length;
		this->z /= length;

		return *this;
	}

	float Dot(Vector3D &src)
	{
		return (
			this->x * src.x + 
			this->y * src.y + 
			this->z * src.z);
	}
};

#pragma pack(pop)

#endif