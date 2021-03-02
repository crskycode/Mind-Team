#include <windows.h>
#include <stdio.h>
#include <sys/stat.h>
#include <malloc.h>
#include <gl/GL.h>
#include  "glext.h"

extern "C"
{
void (APIENTRY *glDeleteObjectARB)(GLhandleARB obj) = NULL;
GLhandleARB (APIENTRY *glGetHandleARB)(GLenum pname) = NULL;
void (APIENTRY *glDetachObjectARB)(GLhandleARB containerObj, GLhandleARB attachedObj) = NULL;
GLhandleARB (APIENTRY *glCreateShaderObjectARB)(GLenum shaderType) = NULL;
void (APIENTRY *glShaderSourceARB)(GLhandleARB shaderObj, GLsizei count, const GLcharARB **string, const GLint *length) = NULL;
void (APIENTRY *glCompileShaderARB)(GLhandleARB shaderObj) = NULL;
GLhandleARB (APIENTRY *glCreateProgramObjectARB)(void) = NULL;
void (APIENTRY *glAttachObjectARB)(GLhandleARB containerObj, GLhandleARB obj) = NULL;
void (APIENTRY *glLinkProgramARB)(GLhandleARB programObj) = NULL;
void (APIENTRY *glUseProgramObjectARB)(GLhandleARB programObj) = NULL;
void (APIENTRY *glValidateProgramARB)(GLhandleARB programObj) = NULL;
void (APIENTRY *glUniform1fARB)(GLint location, GLfloat v0) = NULL;
void (APIENTRY *glUniform2fARB)(GLint location, GLfloat v0, GLfloat v1) = NULL;
void (APIENTRY *glUniform3fARB)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) = NULL;
void (APIENTRY *glUniform4fARB)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) = NULL;
void (APIENTRY *glUniform1iARB)(GLint location, GLint v0) = NULL;
void (APIENTRY *glUniform2iARB)(GLint location, GLint v0, GLint v1) = NULL;
void (APIENTRY *glUniform3iARB)(GLint location, GLint v0, GLint v1, GLint v2) = NULL;
void (APIENTRY *glUniform4iARB)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) = NULL;
void (APIENTRY *glUniform1fvARB)(GLint location, GLsizei count, const GLfloat *value) = NULL;
void (APIENTRY *glUniform2fvARB)(GLint location, GLsizei count, const GLfloat *value) = NULL;
void (APIENTRY *glUniform3fvARB)(GLint location, GLsizei count, const GLfloat *value) = NULL;
void (APIENTRY *glUniform4fvARB)(GLint location, GLsizei count, const GLfloat *value) = NULL;
void (APIENTRY *glUniform1ivARB)(GLint location, GLsizei count, const GLint *value) = NULL;
void (APIENTRY *glUniform2ivARB)(GLint location, GLsizei count, const GLint *value) = NULL;
void (APIENTRY *glUniform3ivARB)(GLint location, GLsizei count, const GLint *value) = NULL;
void (APIENTRY *glUniform4ivARB)(GLint location, GLsizei count, const GLint *value) = NULL;
void (APIENTRY *glUniformMatrix2fvARB)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) = NULL;
void (APIENTRY *glUniformMatrix3fvARB)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) = NULL;
void (APIENTRY *glUniformMatrix4fvARB)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) = NULL;
void (APIENTRY *glGetObjectParameterfvARB)(GLhandleARB obj, GLenum pname, GLfloat *params) = NULL;
void (APIENTRY *glGetObjectParameterivARB)(GLhandleARB obj, GLenum pname, GLint *params) = NULL;
void (APIENTRY *glGetInfoLogARB)(GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog) = NULL;
void (APIENTRY *glGetAttachedObjectsARB)(GLhandleARB containerObj, GLsizei maxCount, GLsizei *count, GLhandleARB *obj) = NULL;
GLint (APIENTRY *glGetUniformLocationARB)(GLhandleARB programObj, const GLcharARB *name) = NULL;
void (APIENTRY *glGetActiveUniformARB)(GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name) = NULL;
void (APIENTRY *glGetUniformfvARB)(GLhandleARB programObj, GLint location, GLfloat *params) = NULL;
void (APIENTRY *glGetUniformivARB)(GLhandleARB programObj, GLint location, GLint *params) = NULL;
void (APIENTRY *glGetShaderSourceARB)(GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *source) = NULL;

void (APIENTRY *glVertexAttrib3fARB)(GLuint index, GLfloat x, GLfloat y, GLfloat z) = NULL;
GLint (APIENTRY *glGetAttribLocationARB)(GLhandleARB programObj, const GLcharARB *name) = NULL;

}

void *COM_LoadFile(const char *filename, int *length)
{
	struct stat _stat;
	stat(filename, &_stat);

	if (_stat.st_size < 1)
		return NULL;

	FILE *fp = fopen(filename, "rb");
	if (!fp)
		return NULL;

	byte *buffer = (byte*)malloc(_stat.st_size + 1);
	if (!buffer)
		return NULL;

	if (fread(buffer, _stat.st_size, 1, fp) != 1)
		return NULL;

	fclose(fp);
	buffer[_stat.st_size] = 0;

	if (length)
		*length = _stat.st_size + 1;

	return buffer;
}

void COM_FreeFile(void *buffer)
{
	free(buffer);
}

int LoadShader(char *vertexShader, char *fragmentShader)
{
	GLhandleARB		hProgram;
	GLcharARB *		szSource;
	GLuint			hShader;
	GLcharARB		szInfoLog[1024];
	GLint			iInfoLen;

	hProgram = glCreateProgramObjectARB();

	if (vertexShader)
	{
		szSource = (GLcharARB *)COM_LoadFile(vertexShader, NULL);

		if (szSource)
		{
			hShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);

			glShaderSourceARB(hShader, 1, (const GLcharARB **)&szSource, NULL);
			glCompileShaderARB(hShader);

			glGetInfoLogARB(hShader, sizeof(szInfoLog), &iInfoLen, szInfoLog);

			COM_FreeFile(szSource);

			if (szInfoLog[0])
			{
				if (strstr(szInfoLog, "ERROR") || strstr(szInfoLog, "error") || strstr(szInfoLog, "Error"))
				{
					return 0;
				}
			}

			glAttachObjectARB(hProgram, hShader);
		}
		else
		{
			return 0;
		}
	}

	if (fragmentShader)
	{
		szSource = (GLcharARB *)COM_LoadFile(fragmentShader, NULL);

		if (szSource)
		{
			hShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

			glShaderSourceARB(hShader, 1, (const GLcharARB **)&szSource, NULL);
			glCompileShaderARB(hShader);

			glGetInfoLogARB(hShader, sizeof(szInfoLog), &iInfoLen, szInfoLog);

			COM_FreeFile(szSource);

			if (szInfoLog[0])
			{
				if (strstr(szInfoLog, "ERROR") || strstr(szInfoLog, "error") || strstr(szInfoLog, "Error"))
				{
					return 0;
				}
			}

			glAttachObjectARB(hProgram, hShader);
		}
		else
		{
			return 0;
		}
	}

	glLinkProgramARB(hProgram);

	return hProgram;
}