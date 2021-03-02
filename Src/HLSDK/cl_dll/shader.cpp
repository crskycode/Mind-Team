/***
 * OpenGL shading program compiler
 * 2017-11-21
***/

#include "cl_dll.h"
#include "qgl.h"


int R_CompileShader(const char *vert_src, const char *frag_src)
{
	char *vert_code = (char *)gEngfuncs.COM_LoadFile(vert_src, 5, NULL);
	char *frag_code = (char *)gEngfuncs.COM_LoadFile(frag_src, 5, NULL);

	if (!vert_code || !frag_code)
	{
		if (vert_code)
			gEngfuncs.COM_FreeFile(vert_code);

		if (frag_code)
			gEngfuncs.COM_FreeFile(frag_code);

		return 0;
	}

	GLuint vert_obj = qglCreateShader(GL_VERTEX_SHADER);
	GLuint frag_obj = qglCreateShader(GL_FRAGMENT_SHADER);

	qglShaderSource(vert_obj, 1, (const char **)&vert_code, NULL);
	qglCompileShader(vert_obj);
	qglShaderSource(frag_obj, 1, (const char **)&frag_code, NULL);
	qglCompileShader(frag_obj);

	gEngfuncs.COM_FreeFile(vert_code);
	gEngfuncs.COM_FreeFile(frag_code);

	GLint vert_stat;
	qglGetShaderiv(vert_obj, GL_COMPILE_STATUS, &vert_stat);
	GLint frag_stat;
	qglGetShaderiv(frag_obj, GL_COMPILE_STATUS, &frag_stat);

	char infolog[512];

	if (!vert_stat)
	{
		qglGetShaderInfoLog(vert_obj, sizeof(infolog), NULL, infolog);
		Sys_Error("Shader: %s\n\n%s\n", vert_src, infolog);
		return 0;
	}

	if (!frag_stat)
	{
		qglGetShaderInfoLog(frag_obj, sizeof(infolog), NULL, infolog);
		Sys_Error("Shader: %s\n\n%s\n", vert_src, infolog);
		return 0;
	}

	GLuint prog = qglCreateProgram();

	qglAttachShader(prog, vert_obj);
	qglAttachShader(prog, frag_obj);

	qglLinkProgram(prog);

	GLint prog_stat;
	qglGetProgramiv(prog, GL_LINK_STATUS, &prog_stat);

	if (!prog_stat)
	{
		qglGetProgramInfoLog(prog, sizeof(infolog), NULL, infolog);
		Sys_Error("Shader: %s\nShader: %s\n\n%s\n", vert_src, frag_src, infolog);
		return 0;
	}

	qglDetachShader(prog, vert_obj);
	qglDeleteShader(vert_obj);
	qglDetachShader(prog, frag_obj);
	qglDeleteShader(frag_obj);

	return prog;
}
