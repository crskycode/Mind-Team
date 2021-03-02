#version 110
#pragma optimize(off)
#pragma debug(on)

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform samplerCube envCubeMap;

uniform float lightAmbient;
uniform float lightDiffuse;
uniform float lightSpecular;

uniform float materialShininess;

// fragment shader input
varying vec3 tangentLightPos;	//light pos in tangent-space.
varying vec3 tangentViewPos;	//view pos in tangent-space.
varying vec3 tangentFragPos;	//fragment pos in tangent-space.

varying vec3 vertexNormal;

varying vec2 TexCoords;		//texture sampler

//#define CUBE_MAPPING

void main()
{
	// Obtain normal from normal map in range [0,1]
	vec3 normal = texture2D(normalMap, TexCoords).rgb;
	//normal = normalize(normal * 2.0 - 1.0);	// this normal is in tangent-space
	normal.x = (normal.x * 2.0) - 1.0;
	normal.y = (normal.y * 2.0) - 1.0;
	
#ifndef CUBE_MAPPING
	// Get diffuse color
	vec3 color = vec3(texture2D(diffuseMap, TexCoords));
	
	// Ambient
	vec3 ambient = lightAmbient * color;
	
	// Diffuse
	vec3 lightDir = normalize(tangentLightPos - tangentFragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = lightDiffuse * diff * color;
	
	// Specular
	vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
	vec3 specular = lightSpecular * spec * vec3(texture2D(specularMap, TexCoords));
	
	// gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
	gl_FragColor = vec4(normal, 1.0);
#else
	normal = normalize(vertexNormal);
	
	float ratio = 1.00 / 0.35;
	vec3 I = normalize(tangentFragPos - tangentViewPos);
	vec3 R = reflect(I, normal);
	//vec3 R = refract(I, normal, ratio);
	
	gl_FragColor = vec4(vec3(textureCube(envCubeMap, R)), 1.0);
#endif
}