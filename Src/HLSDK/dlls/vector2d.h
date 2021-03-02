// 2D Vector

#ifndef VECTOR2D_H
#define VECTOR2D_H

//=========================================================
// 2D Vector
//=========================================================
class Vector2D
{
public:
	// Members
	float x, y;

	// Construction
	Vector2D(void);
	Vector2D(float ix, float iy);
	Vector2D(const float *rgfl);

	// Initialization
	void Init(float ix = 0.0f, float iy = 0.0f);

	// array access...
	float operator[](int i) const;
	float& operator[](int i);

	// Base address...
	operator float *();
	operator const float *() const;

	// equality
	bool operator==(const Vector2D &v) const;
	bool operator!=(const Vector2D &v) const;

	// arithmetic operations
	Vector2D& operator+=(const Vector2D &v);
	Vector2D& operator+=(float fl);
	Vector2D& operator-=(const Vector2D &v);
	Vector2D& operator-=(float fl);
	Vector2D& operator*=(const Vector2D &v);
	Vector2D& operator*=(float s);
	Vector2D& operator/=(const Vector2D &v);
	Vector2D& operator/=(float s);

	// Get the vector's magnitude.
	float Length(void) const;

	// Get the vector's magnitude squared.
	float LengthSqr(void) const;

	// return true if this vector is (0,0,0) within tolerance
	bool IsZero(float tolerance = 0.01f) const;

	// identity of the vector
	Vector2D Normalize(void) const;

	float NormalizeInPlace(void);
	bool IsLengthGreaterThan(float val) const;
	bool IsLengthLessThan(float val) const;

	// Get the distance from this vector to the other one.
	float DistTo(const Vector2D &vOther) const;
	float DistToSqr(const Vector2D &vOther) const;

	// Copy
	void CopyToArray(float *rgfl) const;

	// Dot product.
	float Dot(const Vector2D &vOther) const;

	// assignment
	Vector2D& operator=(const Vector2D &vOther);

	// arithmetic operations
	Vector2D operator-(void) const;

	Vector2D operator+(const Vector2D &v) const;
	Vector2D operator-(const Vector2D &v) const;
	Vector2D operator*(const Vector2D &v) const;
	Vector2D operator*(float fl) const;
	Vector2D operator/(const Vector2D &v) const;
	Vector2D operator/(float fl) const;
};


//-----------------------------------------------------------------------------
//
// Inlined Vector methods
//
//-----------------------------------------------------------------------------

inline Vector2D::Vector2D(void)
{
	x = y = 0.0f;
}

inline Vector2D::Vector2D(float ix, float iy)
{
	x = ix; y = iy;
}

inline Vector2D::Vector2D(const float *rgfl)
{
	x = rgfl[0]; y = rgfl[1];
}

inline void Vector2D::Init(float ix, float iy)
{
	x = ix; y = iy;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline float Vector2D::operator[](int i) const
{
	return ((float *)this)[i];
}

inline float& Vector2D::operator[](int i)
{
	return ((float *)this)[i];
}

//-----------------------------------------------------------------------------
// Base address
//-----------------------------------------------------------------------------
inline Vector2D::operator float *()
{
	return &x;
}

inline Vector2D::operator const float *() const
{
	return &x;
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------
inline bool Vector2D::operator==(const Vector2D &v) const
{
	return (v.x == x) && (v.y == y);
}

inline bool Vector2D::operator!=(const Vector2D &v) const
{
	return (v.x != x) || (v.y != y);
}

//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------
inline Vector2D& Vector2D::operator+=(const Vector2D &v)
{
	x += v.x; y += v.y;
	return *this;
}

inline Vector2D& Vector2D::operator+=(float fl)
{
	x += fl; y += fl;
	return *this;
}

inline Vector2D& Vector2D::operator-=(const Vector2D &v)
{
	x -= v.x; y -= v.y;
	return *this;
}

inline Vector2D& Vector2D::operator-=(float fl)
{
	x -= fl; y -= fl;
	return *this;
}

inline Vector2D& Vector2D::operator*=(const Vector2D &v)
{
	x *= v.x; y *= v.y;
	return *this;
}

inline Vector2D& Vector2D::operator*=(float s)
{
	x *= s; y *= s;
	return *this;
}

inline Vector2D& Vector2D::operator/=(const Vector2D &v)
{
	x /= v.x; y /= v.y;
	return *this;
}

inline Vector2D& Vector2D::operator/=(float s)
{
	x /= s; y /= s;
	return *this;
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
inline float Vector2D::Length(void) const
{
	return sqrt(x*x + y*y);
}

inline float Vector2D::LengthSqr(void) const
{
	return x*x + y*y;
}

//-----------------------------------------------------------------------------
// check
//-----------------------------------------------------------------------------
inline bool Vector2D::IsZero(float tolerance) const
{
	return (x > -tolerance && x < tolerance &&
		    y > -tolerance && y < tolerance);
}

//-----------------------------------------------------------------------------
// identity
//-----------------------------------------------------------------------------
inline Vector2D Vector2D::Normalize(void) const
{
	float radius = sqrt(x*x + y*y);

	// FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero.
	float iradius = 1.0f / (radius + FLT_EPSILON);

	return Vector2D(x*iradius, y*iradius);
}

inline float Vector2D::NormalizeInPlace(void)
{
	float radius = sqrt(x*x + y*y);

	// FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero.
	float iradius = 1.0f / (radius + FLT_EPSILON);

	x *= iradius;
	y *= iradius;

	return radius;
}

inline bool Vector2D::IsLengthGreaterThan(float val) const
{
	return LengthSqr() > val*val;
}

inline bool Vector2D::IsLengthLessThan(float val) const
{
	return LengthSqr() < val*val;
}

inline float Vector2D::DistTo(const Vector2D &vOther) const
{
	return (*this - vOther).Length();
}

inline float Vector2D::DistToSqr(const Vector2D &vOther) const
{
	return (*this - vOther).LengthSqr();
}

//-----------------------------------------------------------------------------
// copy
//-----------------------------------------------------------------------------
inline void Vector2D::CopyToArray(float *rgfl) const
{
	rgfl[0] = x; rgfl[1] = y;
}

//-----------------------------------------------------------------------------
// dot product
//-----------------------------------------------------------------------------
inline float Vector2D::Dot(const Vector2D &vOther) const
{
	return x*vOther.x + y*vOther.y;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------
inline Vector2D& Vector2D::operator=(const Vector2D &vOther)
{
	x = vOther.x; y = vOther.y;
	return *this;
}

//-----------------------------------------------------------------------------
// arithmetic operations
//-----------------------------------------------------------------------------
inline Vector2D Vector2D::operator-(void) const
{
	return Vector2D(-x, -y);
}

inline Vector2D Vector2D::operator+(const Vector2D &v) const
{
	return Vector2D(x+v.x, y+v.y);
}

inline Vector2D Vector2D::operator-(const Vector2D &v) const
{
	return Vector2D(x-v.x, y-v.y);
}

inline Vector2D Vector2D::operator*(const Vector2D &v) const
{
	return Vector2D(x*v.x, y*v.y);
}

inline Vector2D Vector2D::operator*(float fl) const
{
	return Vector2D(x*fl, y*fl);
}

inline Vector2D Vector2D::operator/(const Vector2D &v) const
{
	return Vector2D(x/v.x, y/v.y);
}

inline Vector2D Vector2D::operator/(float fl) const
{
	return Vector2D(x/fl, y/fl);
}

//-----------------------------------------------------------------------------
// Vector related operations
//-----------------------------------------------------------------------------
inline void VectorClear(Vector2D &v)
{
	v.x = v.y = 0.0f;
}

inline void VectorCopy(const Vector2D &src, Vector2D &dst)
{
	dst.x = src.x;
	dst.y = src.y;
}

inline void VectorAdd(const Vector2D &a, const Vector2D &b, Vector2D &c)
{
	c.x = a.x + b.x;
	c.y = a.y + b.y;
}

inline void VectorSubtract(const Vector2D &a, const Vector2D &b, Vector2D &c)
{
	c.x = a.x - b.x;
	c.y = a.y - b.y;
}

inline void VectorScale(const Vector2D &a, float b, Vector2D &c)
{
	c.x = a.x * b;
	c.y = a.y * b;
}

inline void VectorMA(const Vector2D &start, float scale, const Vector2D &direction, Vector2D &dest)
{
	dest.x = start.x + direction.x*scale;
	dest.y = start.y + direction.y*scale;
}

inline float VectorLength(const Vector2D &v)
{
	return sqrt(v.x*v.x + v.y*v.y);
}

inline float DotProduct(const Vector2D &a, const Vector2D &b)
{
	return a.x*b.x + a.y*b.y;
}

#endif