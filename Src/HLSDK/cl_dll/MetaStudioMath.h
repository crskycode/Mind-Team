/**
 * OpenGL vector and matrix
 * 2017-11-21
**/

#ifndef METASTUDIOMATH_H
#define METASTUDIOMATH_H

void GL_VectorSlerp( vec3_t v1, vec3_t v2, float t, vec3_t dest );
void GL_QuaternionSlerp( vec4_t q1, vec4_t q2, float t, vec4_t out );
void GL_AngleMatrix( vec3_t in, mat4_t m );
void GL_QuaternionMatrix( vec4_t q, mat4_t m );
void GL_AxisQuaternion( vec3_t axis, float angle, vec4_t quat );
void GL_QuaternionMul( vec4_t q1, vec4_t q2, vec4_t out );
void GL_ConcatTransforms( mat4_t m1, mat4_t m2, mat4_t out );
void GL_MatrixIdentity( mat4_t m );
void GL_MatrixCopy( mat4_t in, mat4_t out );
void GL_VectorTransform( vec3_t in, mat4_t mat, vec3_t out );

#endif