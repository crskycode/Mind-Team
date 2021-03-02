#ifndef VECTOR2D_H
#define VECTOR2D_H

class Vector2D
{
public:
	Vector2D() : x(0), y(0) {};
	Vector2D(float x, float y) : x(x), y(y) {};
public:
	float x, y;
};

#endif