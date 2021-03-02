#ifndef MATHLIB_H
#define MATHLIB_H

#define _USE_MATH_DEFINES
#include <math.h>

// vector
typedef float vec2_t[2];
typedef float vec3_t[3];
typedef float vec4_t[4];

// matrix
typedef float mat4_t[4][4];


void VectorClear( vec3_t vec );
int VectorCompare( vec3_t vec1, vec3_t vec2 );
int VectorCompare2( vec2_t vec1, vec2_t vec2 );
void VectorCopy( vec3_t src, vec3_t dest );
void VectorCopy2( vec2_t src, vec2_t dest );
void VectorCopy4( vec4_t src, vec4_t dest );
void VectorAdd( vec3_t vec1, vec3_t vec2, vec3_t dest );
void VectorSubtract( vec3_t vec1, vec3_t vec2, vec3_t dest );
void VectorScale( vec3_t vec, float factor, vec3_t dest );
void VectorMA( vec3_t src, float factor, vec3_t dir, vec3_t dest );
float DotProduct( vec3_t vec1, vec3_t vec2 );
void MatrixIdentity( mat4_t dest );
void MatrixCopy( mat4_t src, mat4_t dest );
void MatrixSetupRotate( float angle, vec3_t axis, mat4_t dest );
void GL_AngleQuaternion( vec3_t angles, vec4_t quat );
void GL_QuaternionSlerp( vec4_t quat1, vec4_t quat2, float percent, vec4_t dest );
void GL_AngleMatrix( vec3_t angles, mat4_t mat );
void GL_QuaternionMatrix( vec4_t quat, mat4_t mat );
void GL_MatrixQuaternion( mat4_t mat, vec4_t quat );
void GL_MatrixInverse( mat4_t src, mat4_t dest );
void GL_ConcatTransforms( mat4_t mat1, mat4_t mat2, mat4_t dest );
void GL_VectorTransform( vec3_t src, mat4_t mat, vec3_t dest );

#endif