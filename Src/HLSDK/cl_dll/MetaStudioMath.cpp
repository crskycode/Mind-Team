/**
 * OpenGL vector and matrix
 * 2017-11-21
**/

#include "cl_dll.h"
#include <mathlib.h>


void GL_VectorSlerp( vec3_t v1, vec3_t v2, float t, vec3_t dest )
{
	int i;

	for (i = 0; i < 3; i++)
	{
		dest[i] = v1[i] + t * ( v2[i] - v1[i] );
	}
}

void GL_QuaternionSlerp( vec4_t q1, vec4_t q2, float t, vec4_t out )
{
	float	rot1q[4];
	float	omega, cosom, oosinom;
	float	scalerot0, scalerot1;

	cosom = q1[0]*q2[0] + q1[1]*q2[1] + q1[2]*q2[2] + q1[3]*q2[3];

	if( cosom < 0.0f )
	{
		cosom = -cosom;
		rot1q[0] = -q2[0];
		rot1q[1] = -q2[1];
		rot1q[2] = -q2[2];
		rot1q[3] = -q2[3];
	}
	else  
	{
		rot1q[0] = q2[0];
		rot1q[1] = q2[1];
		rot1q[2] = q2[2];
		rot1q[3] = q2[3];
	}

	if ( ( 1.0f - cosom ) > 0.0001f ) 
	{ 
		omega = acos( cosom );
		oosinom = 1.0f / sin( omega );
		scalerot0 = sin( ( 1.0f - t ) * omega ) * oosinom;
		scalerot1 = sin( t * omega ) * oosinom;
	}
	else
	{ 
		scalerot0 = 1.0f - t;
		scalerot1 = t;
	}

	out[0] = ( scalerot0 * q1[0] + scalerot1 * rot1q[0] );
	out[1] = ( scalerot0 * q1[1] + scalerot1 * rot1q[1] );
	out[2] = ( scalerot0 * q1[2] + scalerot1 * rot1q[2] );
	out[3] = ( scalerot0 * q1[3] + scalerot1 * rot1q[3] );
}

void GL_AngleMatrix( vec3_t in, mat4_t m )
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = in[1] * ( M_PI*2 / 360 );	// YAW
	sy = sin( angle );
	cy = cos( angle );
	angle = in[0] * ( M_PI*2 / 360 );	// PITCH
	sp = sin( angle );
	cp = cos( angle );
	angle = in[2] * ( M_PI*2 / 360 );	// ROLL
	sr = sin( angle );
	cr = cos( angle );

	m[0][0] = cp*cy;
	m[0][1] = cp*sy;
	m[0][2] = -sp;

	m[1][0] = sr*sp*cy+cr*-sy;
	m[1][1] = sr*sp*sy+cr*cy;
	m[1][2] = sr*cp;

	m[2][0] = (cr*sp*cy+-sr*-sy);
	m[2][1] = (cr*sp*sy+-sr*cy);
	m[2][2] = cr*cp;

	m[3][0] = m[3][1] = m[3][2] = m[0][3] = m[1][3] = m[2][3] = 0.0;
	m[3][3] = 1.0;
}

void GL_QuaternionMatrix( vec4_t q, mat4_t m )
{
	float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

	s = 2.0 / ( q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3] );

	xs = q[0] * s;
	ys = q[1] * s;
	zs = q[2] * s;

	wx = q[3] * xs;
	wy = q[3] * ys;
	wz = q[3] * zs;

	xx = q[0] * xs;
	xy = q[0] * ys;
	xz = q[0] * zs;

	yy = q[1] * ys;
	yz = q[1] * zs;

	zz = q[2] * zs;

	m[0][0] = 1.0 - ( yy + zz );
	m[1][0] = xy - wz;
	m[2][0] = xz + wy;
	
	m[0][1] = xy + wz;
	m[1][1] = 1.0 - ( xx + zz );
	m[2][1] = yz - wx;

	m[0][2] = xz - wy;
	m[1][2] = yz + wx;
	m[2][2] = 1.0 - ( xx + yy );

	m[3][0] = m[3][1] = m[3][2] = m[0][3] = m[1][3] = m[2][3] = 0.0;
	m[3][3] = 1.0;
}

void GL_AxisQuaternion( vec3_t axis, float angle, vec4_t quat )
{
	float	a, s, c;

	a = angle * ( M_PI / 180.0 );
	a *= 0.5;

	s = sin( a );
	c = cos( a );

	quat[0] = axis[0] * s;
	quat[1] = axis[1] * s;
	quat[2] = axis[2] * s;
	quat[3] = c;
}

void GL_QuaternionMul( vec4_t q1, vec4_t q2, vec4_t out )
{
	out[0] = q1[3] * q2[0] + q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1];
	out[1] = q1[3] * q2[1] - q1[0] * q2[2] + q1[1] * q2[3] + q1[2] * q2[0];
	out[2] = q1[3] * q2[2] + q1[0] * q2[1] - q1[1] * q2[0] + q1[2] * q2[3];
	out[3] = q1[3] * q2[3] - q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2];
}

void GL_ConcatTransforms( mat4_t m1, mat4_t m2, mat4_t out )
{
	out[0][0] = m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3];
	out[0][1] = m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3];
	out[0][2] = m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3];
	out[0][3] = m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2] + m1[3][3] * m2[0][3];

	out[1][0] = m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3];
	out[1][1] = m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3];
	out[1][2] = m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3];
	out[1][3] = m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2] + m1[3][3] * m2[1][3];

	out[2][0] = m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3];
	out[2][1] = m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3];
	out[2][2] = m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2] + m1[3][2] * m2[2][3];
	out[2][3] = m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1] + m1[2][3] * m2[2][2] + m1[3][3] * m2[2][3];

	out[3][0] = m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2] + m1[3][0] * m2[3][3];
	out[3][1] = m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2] + m1[3][1] * m2[3][3];
	out[3][2] = m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1] + m1[2][2] * m2[3][2] + m1[3][2] * m2[3][3];
	out[3][3] = m1[0][3] * m2[3][0] + m1[1][3] * m2[3][1] + m1[2][3] * m2[3][2] + m1[3][3] * m2[3][3];
}

void GL_MatrixIdentity( mat4_t m )
{
	m[0][0] = 1.0;  m[1][0] = 0.0;  m[2][0] = 0.0;  m[3][0] = 0.0;
	m[0][1] = 0.0;  m[1][1] = 1.0;  m[2][1] = 0.0;  m[3][1] = 0.0;
	m[0][2] = 0.0;  m[1][2] = 0.0;  m[2][2] = 1.0;  m[3][2] = 0.0;
	m[0][3] = 0.0;  m[1][3] = 0.0;  m[2][3] = 0.0;  m[3][3] = 1.0;
}

void GL_MatrixCopy( mat4_t in, mat4_t out )
{
	memcpy( out, in, sizeof( mat4_t ) );
}

void GL_VectorTransform(vec3_t in, mat4_t mat, vec3_t out)
{
	out[0] = mat[0][0] * in[0] + mat[1][0] * in[1] + mat[2][0] * in[2] + mat[3][0];
	out[1] = mat[0][1] * in[0] + mat[1][1] * in[1] + mat[2][1] * in[2] + mat[3][1];
	out[2] = mat[0][2] * in[0] + mat[1][2] * in[1] + mat[2][2] * in[2] + mat[3][2];
}