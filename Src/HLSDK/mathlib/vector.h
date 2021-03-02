#ifndef VECTOR_H
#define VECTOR_H

class Vector
{
public:
	inline void Init(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	inline void operator*=(float s)
	{
		x *= s;
		y *= s;
		z *= s;
	}

	float x, y, z;
};

#endif