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

#define NOVECTOR
#include "extdll.h"

void ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4])
{
	out[0][0] = in1[0][0]*in2[0][0] + in1[0][1]*in2[1][0] + in1[0][2]*in2[2][0];
	out[0][1] = in1[0][0]*in2[0][1] + in1[0][1]*in2[1][1] + in1[0][2]*in2[2][1];
	out[0][2] = in1[0][0]*in2[0][2] + in1[0][1]*in2[1][2] + in1[0][2]*in2[2][2];
	out[0][3] = in1[0][0]*in2[0][3] + in1[0][1]*in2[1][3] + in1[0][2]*in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0]*in2[0][0] + in1[1][1]*in2[1][0] + in1[1][2]*in2[2][0];
	out[1][1] = in1[1][0]*in2[0][1] + in1[1][1]*in2[1][1] + in1[1][2]*in2[2][1];
	out[1][2] = in1[1][0]*in2[0][2] + in1[1][1]*in2[1][2] + in1[1][2]*in2[2][2];
	out[1][3] = in1[1][0]*in2[0][3] + in1[1][1]*in2[1][3] + in1[1][2]*in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0]*in2[0][0] + in1[2][1]*in2[1][0] + in1[2][2]*in2[2][0];
	out[2][1] = in1[2][0]*in2[0][1] + in1[2][1]*in2[1][1] + in1[2][2]*in2[2][1];
	out[2][2] = in1[2][0]*in2[0][2] + in1[2][1]*in2[1][2] + in1[2][2]*in2[2][2];
	out[2][3] = in1[2][0]*in2[0][3] + in1[2][1]*in2[1][3] + in1[2][2]*in2[2][3] + in1[2][3];
}

void AngleQuaternion(float *angles, vec4_t quaternion)
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[2] * 0.5;
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[1] * 0.5;
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[0] * 0.5;
	sr = sin(angle);
	cr = cos(angle);

	quaternion[0] = sr*cp*cy - cr*sp*sy;
	quaternion[1] = cr*sp*cy + sr*cp*sy;
	quaternion[2] = cr*cp*sy - sr*sp*cy;
	quaternion[3] = cr*cp*cy + sr*sp*sy;
}

void QuaternionSlerp(vec4_t p, vec4_t q, float t, vec4_t qt)
{
	int		i;
	float	omega, cosom, sinom, sclp, sclq;

	float a = 0;
	float b = 0;

	for (i = 0; i < 4; i++)
	{
		a += (p[i]-q[i]) * (p[i]-q[i]);
		b += (p[i]+q[i]) * (p[i]+q[i]);
	}

	if (a > b)
	{
		for (i = 0; i < 4; i++)
		{
			q[i] = -q[i];
		}
	}

	cosom = p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3];

	if ((1.0 + cosom) > 0.000001)
	{
		if ((1.0 - cosom) > 0.000001)
		{
			omega = acos(cosom);
			sinom = sin(omega);
			sclp = sin((1.0 - t)*omega) / sinom;
			sclq = sin(t*omega) / sinom;
		}
		else
		{
			sclp = 1.0 - t;
			sclq = t;
		}

		for (i = 0; i < 4; i++)
		{
			qt[i] = sclp * p[i] + sclq * q[i];
		}
	}
	else
	{
		qt[0] = -q[1];
		qt[1] = q[0];
		qt[2] = -q[3];
		qt[3] = q[2];
		sclp = sin((1.0 - t) * (0.5 * M_PI));
		sclq = sin(t * (0.5 * M_PI));

		for (i = 0; i < 3; i++)
		{
			qt[i] = sclp * p[i] + sclq * qt[i];
		}
	}
}

void QuaternionMatrix(vec4_t quaternion, float (*matrix)[4])
{
	matrix[0][0] = 1.0 - 2.0 * quaternion[1] * quaternion[1] - 2.0 * quaternion[2] * quaternion[2];
	matrix[1][0] = 2.0 * quaternion[0] * quaternion[1] + 2.0 * quaternion[3] * quaternion[2];
	matrix[2][0] = 2.0 * quaternion[0] * quaternion[2] - 2.0 * quaternion[3] * quaternion[1];

	matrix[0][1] = 2.0 * quaternion[0] * quaternion[1] - 2.0 * quaternion[3] * quaternion[2];
	matrix[1][1] = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[2] * quaternion[2];
	matrix[2][1] = 2.0 * quaternion[1] * quaternion[2] + 2.0 * quaternion[3] * quaternion[0];

	matrix[0][2] = 2.0 * quaternion[0] * quaternion[2] + 2.0 * quaternion[3] * quaternion[1];
	matrix[1][2] = 2.0 * quaternion[1] * quaternion[2] - 2.0 * quaternion[3] * quaternion[0];
	matrix[2][2] = 1.0 - 2.0 * quaternion[0] * quaternion[0] - 2.0 * quaternion[1] * quaternion[1];
}

void MatrixCopy( float in[3][4], float out[3][4] )
{
	out[0][0] = in[0][0];
	out[1][0] = in[1][0];
	out[2][0] = in[2][0];

	out[0][1] = in[0][1];
	out[1][1] = in[1][1];
	out[2][1] = in[2][1];

	out[0][2] = in[0][2];
	out[2][2] = in[2][2];
	out[1][2] = in[1][2];

	out[0][3] = in[0][3];
	out[1][3] = in[1][3];
	out[2][3] = in[2][3];
}