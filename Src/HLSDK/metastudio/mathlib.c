
#include "mathlib.h"
#include <float.h>
#include <memory.h>

// conversion from 'double' to 'float'
#pragma warning( disable:4244 )


void VectorClear( vec3_t vec )
{
	vec[0] = 0.0f;
	vec[1] = 0.0f;
	vec[2] = 0.0f;
}

int VectorCompare( vec3_t vec1, vec3_t vec2 )
{
	int		i;

	for ( i = 0; i < 3; i++ )
	{
		if ( fabs( vec1[i] - vec2[i] ) > FLT_EPSILON )
		{
			return 0;
		}
	}

	return 1;
}

int VectorCompare2( vec2_t vec1, vec2_t vec2 )
{
	int		i;

	for ( i = 0; i < 2; i++ )
	{
		if ( fabs( vec1[i] - vec2[i] ) > FLT_EPSILON )
		{
			return 0;
		}
	}

	return 1;
}

void VectorCopy( vec3_t src, vec3_t dest )
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
}

void VectorCopy2( vec2_t src, vec2_t dest )
{
	dest[0] = src[0];
	dest[1] = src[1];
}

void VectorCopy4( vec4_t src, vec4_t dest )
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	dest[3] = src[3];
}

void VectorAdd( vec3_t vec1, vec3_t vec2, vec3_t dest )
{
	dest[0] = vec1[0] + vec2[0];
	dest[1] = vec1[1] + vec2[1];
	dest[2] = vec1[2] + vec2[2];
}

void VectorSubtract( vec3_t vec1, vec3_t vec2, vec3_t dest )
{
	dest[0] = vec1[0] - vec2[0];
	dest[1] = vec1[1] - vec2[1];
	dest[2] = vec1[2] - vec2[2];
}

void VectorScale( vec3_t vec, float factor, vec3_t dest )
{
	dest[0] = vec[0] * factor;
	dest[1] = vec[1] * factor;
	dest[2] = vec[2] * factor;
}

void VectorMA( vec3_t src, float factor, vec3_t dir, vec3_t dest )
{
	dest[0] = src[0] + dir[0] * factor;
	dest[1] = src[1] + dir[1] * factor;
	dest[2] = src[2] + dir[2] * factor;
}

float DotProduct( vec3_t vec1, vec3_t vec2 )
{
	return vec1[0]*vec2[0] + vec1[1]*vec2[1] + vec1[2]*vec2[2];
}

void GL_AngleQuaternion( vec3_t angles, vec4_t quat )
{
	float	angle;
	float	t0, t1, t2, t3, t4, t5;

	angle = angles[2] * 0.5f;
	t0 = cos( angle );
	t1 = sin( angle );
	angle = angles[0] * 0.5f;
	t2 = cos( angle );
	t3 = sin( angle );
	angle = angles[1] * 0.5f;
	t4 = cos( angle );
	t5 = sin( angle );

	quat[0] = t0 * t3 * t4 - t1 * t2 * t5;
	quat[1] = t0 * t2 * t5 + t1 * t3 * t4;
	quat[2] = t1 * t2 * t4 - t0 * t3 * t5;
	quat[3] = t0 * t2 * t4 + t1 * t3 * t5;
}

void GL_QuaternionSlerp( vec4_t quat1, vec4_t quat2, float t, vec4_t dest )
{
	float	rot1q[4];
	float	omega, cosom, oosinom;
	float	scalerot0, scalerot1;

	cosom = quat1[0]*quat2[0] + quat1[1]*quat2[1] + quat1[2]*quat2[2] + quat1[3]*quat2[3];

	if( cosom < 0.0f )
	{
		cosom = -cosom;
		rot1q[0] = -quat2[0];
		rot1q[1] = -quat2[1];
		rot1q[2] = -quat2[2];
		rot1q[3] = -quat2[3];
	}
	else  
	{
		rot1q[0] = quat2[0];
		rot1q[1] = quat2[1];
		rot1q[2] = quat2[2];
		rot1q[3] = quat2[3];
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

	dest[0] = ( scalerot0 * quat1[0] + scalerot1 * rot1q[0] );
	dest[1] = ( scalerot0 * quat1[1] + scalerot1 * rot1q[1] );
	dest[2] = ( scalerot0 * quat1[2] + scalerot1 * rot1q[2] );
	dest[3] = ( scalerot0 * quat1[3] + scalerot1 * rot1q[3] );
}

/*********

 The GL matrix

 | Xx Yx Zx Tx |
 | Xy Yy Zy Ty |
 | Xz Yz Zz Tz |
 |  0  0  0  1 |

 This matrix store by column :

  m[0] = [ Xx Xy Xz 0 ];
  m[1] = [ Yx Yy Yz 0 ];
  m[2] = [ Zx Zy Zz 0 ];
  m[3] = [ Tx Ty Tz 1 ];

*********/

void MatrixIdentity( mat4_t dest )
{
	dest[0][0] = 1.0;  dest[1][0] = 0.0;  dest[2][0] = 0.0;  dest[3][0] = 0.0;
	dest[0][1] = 0.0;  dest[1][1] = 1.0;  dest[2][1] = 0.0;  dest[3][1] = 0.0;
	dest[0][2] = 0.0;  dest[1][2] = 0.0;  dest[2][2] = 1.0;  dest[3][2] = 0.0;
	dest[0][3] = 0.0;  dest[1][3] = 0.0;  dest[2][3] = 0.0;  dest[3][3] = 1.0;
}

void MatrixCopy( mat4_t src, mat4_t dest )
{
	memcpy( dest, src, sizeof( mat4_t ) );
}

void MatrixSetupRotate( float angle, vec3_t axis, mat4_t dest )
{
	float c, s;
	vec3_t temp;

	c = cos( angle );
	s = sin( angle );

	temp[0] = ( 1.0 - c ) * axis[0];
	temp[1] = ( 1.0 - c ) * axis[1];
	temp[2] = ( 1.0 - c ) * axis[2];

	dest[0][0] = c + temp[0] * axis[0];
	dest[0][1] = temp[0] * axis[1] + s * axis[2];
	dest[0][2] = temp[0] * axis[2] - s * axis[1];

	dest[1][0] = temp[1] * axis[0] - s * axis[2];
	dest[1][1] = c + temp[1] * axis[1];
	dest[1][2] = temp[1] * axis[2] + s * axis[0];

	dest[2][0] = temp[2] * axis[0] + s * axis[1];
	dest[2][1] = temp[2] * axis[1] - s * axis[0];
	dest[2][2] = c + temp[2] * axis[2];

	dest[0][3] = 0.0;
	dest[1][3] = 0.0;
	dest[2][3] = 0.0;

	dest[3][0] = 0.0;
	dest[3][1] = 0.0;
	dest[3][2] = 0.0;
	dest[3][3] = 1.0;
}

void GL_AngleMatrix(vec3_t angles, mat4_t mat)
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[1] * (M_PI * 2 / 360);	// YAW
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[0] * (M_PI * 2 / 360);	// PITCH
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[2] * (M_PI * 2 / 360);	// ROLL
	sr = sin(angle);
	cr = cos(angle);

	mat[0][0] = cp * cy;
	mat[0][1] = cp * sy;
	mat[0][2] = -sp;

	mat[1][0] = sr * sp*cy + cr * -sy;
	mat[1][1] = sr * sp*sy + cr * cy;
	mat[1][2] = sr * cp;

	mat[2][0] = (cr*sp*cy + -sr * -sy);
	mat[2][1] = (cr*sp*sy + -sr * cy);
	mat[2][2] = cr * cp;

	mat[3][0] = mat[3][1] = mat[3][2] = mat[0][3] = mat[1][3] = mat[2][3] = 0.0;
	mat[3][3] = 1.0;
}

void GL_QuaternionMatrix( vec4_t quat, mat4_t mat )
{
	float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

	s = 2.0f / ( quat[0]*quat[0] + quat[1]*quat[1] + quat[2]*quat[2] + quat[3]*quat[3] );

	xs = quat[0] * s;
	ys = quat[1] * s;
	zs = quat[2] * s;

	wx = quat[3] * xs;
	wy = quat[3] * ys;
	wz = quat[3] * zs;

	xx = quat[0] * xs;
	xy = quat[0] * ys;
	xz = quat[0] * zs;

	yy = quat[1] * ys;
	yz = quat[1] * zs;

	zz = quat[2] * zs;

	mat[0][0] = 1.0f - ( yy + zz );
	mat[1][0] = xy - wz;
	mat[2][0] = xz + wy;
	
	mat[0][1] = xy + wz;
	mat[1][1] = 1.0f - ( xx + zz );
	mat[2][1] = yz - wx;

	mat[0][2] = xz - wy;
	mat[1][2] = yz + wx;
	mat[2][2] = 1.0f - ( xx + yy );

	mat[3][0] = mat[3][1] = mat[3][2] = mat[0][3] = mat[1][3] = mat[2][3] = 0.0f;
	mat[3][3] = 1.0f;
}

#define QX  0
#define QY  1
#define QZ  2
#define QW  3

static int g_QNext[3] = { QY, QZ, QX };

void quat_ConvertFromMatrix(float *pQuat, const float mat[4][4])
{
	float diag, s;
	int i, j, k;


	diag = mat[0][0] + mat[1][1] + mat[2][2];

	if (diag < -0.999f)
	{
		i = QX;
		if (mat[QY][QY] > mat[QX][QX])
			i = QY;
		if (mat[QZ][QZ] > mat[i][i])
			i = QZ;

		j = g_QNext[i];
		k = g_QNext[j];

		s = sqrtf(mat[i][i] - (mat[j][j] + mat[k][k]) + /*mat[3][3]*/ 1.0f);

		pQuat[i] = s * 0.5f;
		s = 0.5f / s;
		pQuat[QW] = (mat[k][j] - mat[j][k]) * s;
		pQuat[j] = (mat[j][i] + mat[i][j]) * s;
		pQuat[k] = (mat[k][i] + mat[i][k]) * s;
		return;
	}

	s = sqrtf(diag + /*mat[3][3]*/ 1.0f);

	pQuat[3] = s * 0.5f;
	s = 0.5f / s;

	pQuat[0] = (mat[2][1] - mat[1][2]) * s;
	pQuat[1] = (mat[0][2] - mat[2][0]) * s;
	pQuat[2] = (mat[1][0] - mat[0][1]) * s;
}

void GL_MatrixQuaternion( mat4_t mat, vec4_t quat )
{
	float fourXSquaredMinus1;
	float fourYSquaredMinus1;
	float fourZSquaredMinus1;
	float fourWSquaredMinus1;
	
	int biggestIndex;
	float fourBiggestSquaredMinus1;
	
	float biggestVal;
	float mult;
	
	fourXSquaredMinus1 = mat[0][0] - mat[1][1] - mat[2][2];
	fourYSquaredMinus1 = mat[1][1] - mat[0][0] - mat[2][2];
	fourZSquaredMinus1 = mat[2][2] - mat[0][0] - mat[1][1];
	fourWSquaredMinus1 = mat[0][0] + mat[1][1] + mat[2][2];
	
	biggestIndex = 0;
	fourBiggestSquaredMinus1 = fourWSquaredMinus1;
	
	if( fourXSquaredMinus1 > fourBiggestSquaredMinus1 )
	{
		fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		biggestIndex = 1;
	}
	if( fourYSquaredMinus1 > fourBiggestSquaredMinus1 )
	{
		fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		biggestIndex = 2;
	}
	if( fourZSquaredMinus1 > fourBiggestSquaredMinus1 )
	{
		fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		biggestIndex = 3;
	}
	
	biggestVal = sqrt( fourBiggestSquaredMinus1 + 1.0 ) * 0.5;
	mult = 0.25 / biggestVal;
	
	switch( biggestIndex )
	{
	case 0:
		quat[3] = biggestVal;
		quat[0] = ( mat[1][2] - mat[2][1] ) * mult;
		quat[1] = ( mat[2][0] - mat[0][2] ) * mult;
		quat[2] = ( mat[0][1] - mat[1][0] ) * mult;
		break;
	case 1:
		quat[3] = ( mat[1][2] - mat[2][1]) * mult;
		quat[0] = biggestVal;
		quat[1] = ( mat[0][1] + mat[1][0] ) * mult;
		quat[2] = ( mat[2][0] + mat[0][2] ) * mult;
		break;
	case 2:
		quat[3] = ( mat[2][0] - mat[0][2] ) * mult;
		quat[0] = ( mat[0][1] + mat[1][0] ) * mult;
		quat[1] = biggestVal;
		quat[2] = ( mat[1][2] + mat[2][1] ) * mult;
		break;
	case 3:
		quat[3] = ( mat[0][1] - mat[1][0] ) * mult;
		quat[0] = ( mat[2][0] + mat[0][2] ) * mult;
		quat[1] = ( mat[1][2] + mat[2][1] ) * mult;
		quat[2] = biggestVal;
		break;
	}
}

void GL_MatrixInverse( mat4_t src, mat4_t dest )
{
	int		i, j;
	float	coef00, coef02, coef03, coef04, coef06, coef07;
	float	coef08, coef10, coef11, coef12, coef14, coef15;
	float	coef16, coef18, coef19, coef20, coef22, coef23;
	vec4_t	fac0, fac1, fac2, fac3, fac4, fac5;
	vec4_t	vec0, vec1, vec2, vec3;
	vec4_t	inv0, inv1, inv2, inv3;
	mat4_t	inverse;
	float	determinant;

	coef00 = src[2][2] * src[3][3] - src[3][2] * src[2][3];
	coef02 = src[1][2] * src[3][3] - src[3][2] * src[1][3];
	coef03 = src[1][2] * src[2][3] - src[2][2] * src[1][3];
	coef04 = src[2][1] * src[3][3] - src[3][1] * src[2][3];
	coef06 = src[1][1] * src[3][3] - src[3][1] * src[1][3];
	coef07 = src[1][1] * src[2][3] - src[2][1] * src[1][3];
	coef08 = src[2][1] * src[3][2] - src[3][1] * src[2][2];
	coef10 = src[1][1] * src[3][2] - src[3][1] * src[1][2];
	coef11 = src[1][1] * src[2][2] - src[2][1] * src[1][2];
	coef12 = src[2][0] * src[3][3] - src[3][0] * src[2][3];
	coef14 = src[1][0] * src[3][3] - src[3][0] * src[1][3];
	coef15 = src[1][0] * src[2][3] - src[2][0] * src[1][3];
	coef16 = src[2][0] * src[3][2] - src[3][0] * src[2][2];
	coef18 = src[1][0] * src[3][2] - src[3][0] * src[1][2];
	coef19 = src[1][0] * src[2][2] - src[2][0] * src[1][2];
	coef20 = src[2][0] * src[3][1] - src[3][0] * src[2][1];
	coef22 = src[1][0] * src[3][1] - src[3][0] * src[1][1];
	coef23 = src[1][0] * src[2][1] - src[2][0] * src[1][1];

	fac0[0] = coef00;  fac0[1] = coef00;  fac0[2] = coef02;  fac0[3] = coef03;
	fac1[0] = coef04;  fac1[1] = coef04;  fac1[2] = coef06;  fac1[3] = coef07;
	fac2[0] = coef08;  fac2[1] = coef08;  fac2[2] = coef10;  fac2[3] = coef11;
	fac3[0] = coef12;  fac3[1] = coef12;  fac3[2] = coef14;  fac3[3] = coef15;
	fac4[0] = coef16;  fac4[1] = coef16;  fac4[2] = coef18;  fac4[3] = coef19;
	fac5[0] = coef20;  fac5[1] = coef20;  fac5[2] = coef22;  fac5[3] = coef23;

	vec0[0] = src[1][0];  vec0[1] = src[0][0];  vec0[2] = src[0][0];  vec0[3] = src[0][0];
	vec1[0] = src[1][1];  vec1[1] = src[0][1];  vec1[2] = src[0][1];  vec1[3] = src[0][1];
	vec2[0] = src[1][2];  vec2[1] = src[0][2];  vec2[2] = src[0][2];  vec2[3] = src[0][2];
	vec3[0] = src[1][3];  vec3[1] = src[0][3];  vec3[2] = src[0][3];  vec3[3] = src[0][3];

	for ( i = 0, j = 1; i < 4; i++, j = -j )
	{
		inv0[i] =  j * ( vec1[i] * fac0[i] - vec2[i] * fac1[i] + vec3[i] * fac2[i] );
		inv1[i] = -j * ( vec0[i] * fac0[i] - vec2[i] * fac3[i] + vec3[i] * fac4[i] );
		inv2[i] =  j * ( vec0[i] * fac1[i] - vec1[i] * fac3[i] + vec3[i] * fac5[i] );
		inv3[i] = -j * ( vec0[i] * fac2[i] - vec1[i] * fac4[i] + vec2[i] * fac5[i] );
	}

	for ( i = 0; i < 4; i++ )
	{
		inverse[0][i] = inv0[i];
		inverse[1][i] = inv1[i];
		inverse[2][i] = inv2[i];
		inverse[3][i] = inv3[i];
	}

	for ( i = 0, determinant = 0; i < 4; i++ )
	{
		determinant += src[0][i] * inverse[i][0];
	}

	for ( i = 0; i < 4; i++ )
	{
		for ( j = 0; j < 4; j++ )
		{
			dest[i][j] = inverse[i][j] / determinant;
			dest[i][j] = inverse[i][j] / determinant;
			dest[i][j] = inverse[i][j] / determinant;
			dest[i][j] = inverse[i][j] / determinant;
			dest[i][j] = inverse[i][j] / determinant;
			dest[i][j] = inverse[i][j] / determinant;
			dest[i][j] = inverse[i][j] / determinant;
			dest[i][j] = inverse[i][j] / determinant;
			dest[i][j] = inverse[i][j] / determinant;
			dest[i][j] = inverse[i][j] / determinant;
			dest[i][j] = inverse[i][j] / determinant;
			dest[i][j] = inverse[i][j] / determinant;
			dest[i][j] = inverse[i][j] / determinant;
			dest[i][j] = inverse[i][j] / determinant;
			dest[i][j] = inverse[i][j] / determinant;
			dest[i][j] = inverse[i][j] / determinant;
		}
	}

	/*int		i, j;

	for ( i = 0; i < 4; i++ )
	{
		for ( j = 0; j < 4; j++ )
		{
			dest[j][i] = src[i][j];
		}
	}*/
}

void GL_ConcatTransforms( mat4_t mat1, mat4_t mat2, mat4_t dest )
{
	dest[0][0] = mat1[0][0] * mat2[0][0] + mat1[1][0] * mat2[0][1] + mat1[2][0] * mat2[0][2] + mat1[3][0] * mat2[0][3];
	dest[0][1] = mat1[0][1] * mat2[0][0] + mat1[1][1] * mat2[0][1] + mat1[2][1] * mat2[0][2] + mat1[3][1] * mat2[0][3];
	dest[0][2] = mat1[0][2] * mat2[0][0] + mat1[1][2] * mat2[0][1] + mat1[2][2] * mat2[0][2] + mat1[3][2] * mat2[0][3];
	dest[0][3] = mat1[0][3] * mat2[0][0] + mat1[1][3] * mat2[0][1] + mat1[2][3] * mat2[0][2] + mat1[3][3] * mat2[0][3];

	dest[1][0] = mat1[0][0] * mat2[1][0] + mat1[1][0] * mat2[1][1] + mat1[2][0] * mat2[1][2] + mat1[3][0] * mat2[1][3];
	dest[1][1] = mat1[0][1] * mat2[1][0] + mat1[1][1] * mat2[1][1] + mat1[2][1] * mat2[1][2] + mat1[3][1] * mat2[1][3];
	dest[1][2] = mat1[0][2] * mat2[1][0] + mat1[1][2] * mat2[1][1] + mat1[2][2] * mat2[1][2] + mat1[3][2] * mat2[1][3];
	dest[1][3] = mat1[0][3] * mat2[1][0] + mat1[1][3] * mat2[1][1] + mat1[2][3] * mat2[1][2] + mat1[3][3] * mat2[1][3];

	dest[2][0] = mat1[0][0] * mat2[2][0] + mat1[1][0] * mat2[2][1] + mat1[2][0] * mat2[2][2] + mat1[3][0] * mat2[2][3];
	dest[2][1] = mat1[0][1] * mat2[2][0] + mat1[1][1] * mat2[2][1] + mat1[2][1] * mat2[2][2] + mat1[3][1] * mat2[2][3];
	dest[2][2] = mat1[0][2] * mat2[2][0] + mat1[1][2] * mat2[2][1] + mat1[2][2] * mat2[2][2] + mat1[3][2] * mat2[2][3];
	dest[2][3] = mat1[0][3] * mat2[2][0] + mat1[1][3] * mat2[2][1] + mat1[2][3] * mat2[2][2] + mat1[3][3] * mat2[2][3];

	dest[3][0] = mat1[0][0] * mat2[3][0] + mat1[1][0] * mat2[3][1] + mat1[2][0] * mat2[3][2] + mat1[3][0] * mat2[3][3];
	dest[3][1] = mat1[0][1] * mat2[3][0] + mat1[1][1] * mat2[3][1] + mat1[2][1] * mat2[3][2] + mat1[3][1] * mat2[3][3];
	dest[3][2] = mat1[0][2] * mat2[3][0] + mat1[1][2] * mat2[3][1] + mat1[2][2] * mat2[3][2] + mat1[3][2] * mat2[3][3];
	dest[3][3] = mat1[0][3] * mat2[3][0] + mat1[1][3] * mat2[3][1] + mat1[2][3] * mat2[3][2] + mat1[3][3] * mat2[3][3];
}

void GL_VectorTransform( vec3_t src, mat4_t mat, vec3_t dest )
{
	dest[0] = mat[0][0] * src[0] + mat[1][0] * src[1] + mat[2][0] * src[2] + mat[3][0];
	dest[1] = mat[0][1] * src[0] + mat[1][1] * src[1] + mat[2][1] * src[2] + mat[3][1];
	dest[2] = mat[0][2] * src[0] + mat[1][2] * src[1] + mat[2][2] * src[2] + mat[3][2];
}