#ifndef SHADER_H
#define SHADER_H

#define SHADER_DECLARE(name) name##_shader_t name;

#define SHADER_UNIFORM(name, uniform) name.uniform = glGetUniformLocationARB(name.program, #uniform);

typedef struct
{
	GLhandleARB program;

	GLint diffuseMap;
	GLint specularMap;
	GLint normalMap;

	GLint lightAmbient;
	GLint lightDiffuse;
	GLint lightSpecular;

	GLint materialShininess;
}
studio_shader_t;

#endif