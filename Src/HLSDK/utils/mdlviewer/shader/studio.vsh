#version 110
#extension GL_ARB_gpu_shader5 : enable
#pragma optimize(off)
#pragma debug(on)

uniform vec3 lightPos;
uniform vec3 viewPos;

// vertex shader input
attribute vec3 tangent;
attribute vec3 bitangent;
// attribute vec3 normal;

// vertex shader output
varying vec3 tangentLightPos;	//light pos in tangent-space
varying vec3 tangentViewPos;	//view pos in tangent-space
varying vec3 tangentFragPos;	//fragment pos in tangent-space

varying vec2 TexCoords;		//texture sampler

varying vec3 vertexNormal;

void main()
{
	gl_Position = ftransform();
	
	vec3 fragPos = vec3(gl_ModelViewMatrix * vec4(gl_Vertex.xyz / gl_Vertex.w, 1.0));
	
	vec3 T = normalize(gl_NormalMatrix * tangent);
	vec3 N = normalize(gl_NormalMatrix * gl_Normal);
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	// then retrieve perpendicular vector B with the cross product of T and N
	vec3 B = normalize(cross(N, T));
	//vec3 B = normalize(gl_NormalMatrix * bitangent);
	
	// this matrix transform world-space to tangent-space
	mat3 TBN = inverse(mat3(T, B, N));
	
	tangentLightPos = TBN * lightPos;
	tangentViewPos = TBN * viewPos;
	tangentFragPos = TBN * fragPos;
	
	vertexNormal = N;
	
	TexCoords = vec2(gl_MultiTexCoord0);
}