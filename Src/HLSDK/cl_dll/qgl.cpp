/**
 * Dynamically link to the OpenGL library
 * 2017-11-20
**/

#include "cl_dll.h"
#include "qgl.h"
#pragma comment(lib, "opengl32.lib")

// OpenGL wgl prototypes
PFNWGLGETPROCADDRESSPROC qwglGetProcAddress = NULL;

/* GL_VERSION_1_0 */
PFNGLBINDTEXTUREPROC qglBindTexture = NULL;
PFNGLDEPTHMASKPROC qglDepthMask = NULL;
PFNGLDRAWELEMENTSPROC qglDrawElements = NULL;
PFNGLFRUSTUMPROC qglFrustum = NULL;
PFNGLLOADIDENTITYPROC qglLoadIdentity = NULL;
PFNGLMATRIXMODEPROC qglMatrixMode = NULL;
PFNGLPOPMATRIXPROC qglPopMatrix = NULL;
PFNGLPUSHMATRIXPROC qglPushMatrix = NULL;
PFNGLREADPIXELSPROC qglReadPixels = NULL;
PFNGLTEXIMAGE2DPROC qglTexImage2D = NULL;
PFNGLTEXPARAMETERIPROC qglTexParameteri = NULL;
PFNGLTEXSUBIMAGE2DPROC qglTexSubImage2D = NULL;

/* GL_VERSION_1_5 */
PFNGLBINDBUFFERPROC qglBindBuffer = NULL;
PFNGLBUFFERDATAPROC qglBufferData = NULL;
PFNGLDELETEBUFFERSPROC qglDeleteBuffers = NULL;
PFNGLGENBUFFERSPROC qglGenBuffers = NULL;
PFNGLMAPBUFFERPROC qglMapBuffer = NULL;
PFNGLUNMAPBUFFERPROC qglUnmapBuffer = NULL;

/* GL_VERSION_2_0 */
PFNGLATTACHSHADERPROC qglAttachShader = NULL;
PFNGLCOMPILESHADERPROC qglCompileShader = NULL;
PFNGLCREATEPROGRAMPROC qglCreateProgram = NULL;
PFNGLCREATESHADERPROC qglCreateShader = NULL;
PFNGLDELETEPROGRAMPROC qglDeleteProgram = NULL;
PFNGLDELETESHADERPROC qglDeleteShader = NULL;
PFNGLDETACHSHADERPROC qglDetachShader = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC qglDisableVertexAttribArray = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC qglEnableVertexAttribArray = NULL;
PFNGLGETATTRIBLOCATIONPROC qglGetAttribLocation = NULL;
PFNGLGETPROGRAMINFOLOGPROC qglGetProgramInfoLog = NULL;
PFNGLGETPROGRAMIVPROC qglGetProgramiv = NULL;
PFNGLGETSHADERINFOLOGPROC qglGetShaderInfoLog = NULL;
PFNGLGETSHADERIVPROC qglGetShaderiv = NULL;
PFNGLGETUNIFORMLOCATIONPROC qglGetUniformLocation = NULL;
PFNGLLINKPROGRAMPROC qglLinkProgram = NULL;
PFNGLSHADERSOURCEPROC qglShaderSource = NULL;
PFNGLUNIFORM1IPROC qglUniform1i = NULL;
PFNGLUNIFORMMATRIX4FVPROC qglUniformMatrix4fv = NULL;
PFNGLUSEPROGRAMPROC qglUseProgram = NULL;
PFNGLVERTEXATTRIBPOINTERPROC qglVertexAttribPointer = NULL;


void QGL_Init(void)
{
	HMODULE hOpenGL = GetModuleHandle("opengl32.dll");

	if (!hOpenGL)
	{
		Sys_Error("Couldn't to link OpenGL library");
		return;
	}

	// OpenGL wgl
	qwglGetProcAddress = (PFNWGLGETPROCADDRESSPROC)GetProcAddress(hOpenGL, "wglGetProcAddress");

	// GL_VERSION_1_0
	qglBindTexture = (PFNGLBINDTEXTUREPROC)GetProcAddress(hOpenGL, "glBindTexture");
	qglDepthMask = (PFNGLDEPTHMASKPROC)GetProcAddress(hOpenGL, "glDepthMask");
	qglDrawElements = (PFNGLDRAWELEMENTSPROC)GetProcAddress(hOpenGL, "glDrawElements");
	qglFrustum = (PFNGLFRUSTUMPROC)GetProcAddress(hOpenGL, "glFrustum");
	qglLoadIdentity = (PFNGLLOADIDENTITYPROC)GetProcAddress(hOpenGL, "glLoadIdentity");
	qglMatrixMode = (PFNGLMATRIXMODEPROC)GetProcAddress(hOpenGL, "glMatrixMode");
	qglPopMatrix = (PFNGLPOPMATRIXPROC)GetProcAddress(hOpenGL, "glPopMatrix");
	qglPushMatrix = (PFNGLPUSHMATRIXPROC)GetProcAddress(hOpenGL, "glPushMatrix");
	qglReadPixels = (PFNGLREADPIXELSPROC)GetProcAddress(hOpenGL, "glReadPixels");
	qglTexImage2D = (PFNGLTEXIMAGE2DPROC)GetProcAddress(hOpenGL, "glTexImage2D");
	qglTexParameteri = (PFNGLTEXPARAMETERIPROC)GetProcAddress(hOpenGL, "glTexParameteri");
	qglTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC)GetProcAddress(hOpenGL, "glTexSubImage2D");

	// GL_VERSION_1_5
	qglBindBuffer = (PFNGLBINDBUFFERPROC)qwglGetProcAddress("glBindBuffer");
	qglBufferData = (PFNGLBUFFERDATAPROC)qwglGetProcAddress("glBufferData");
	qglDeleteBuffers = (PFNGLDELETEBUFFERSPROC)qwglGetProcAddress("glDeleteBuffers");
	qglGenBuffers = (PFNGLGENBUFFERSPROC)qwglGetProcAddress("glGenBuffers");
	qglMapBuffer = (PFNGLMAPBUFFERPROC)qwglGetProcAddress("glMapBuffer");
	qglUnmapBuffer = (PFNGLUNMAPBUFFERPROC)qwglGetProcAddress("glUnmapBuffer");

	// GL_VERSION_2_0
	qglAttachShader = (PFNGLATTACHSHADERPROC)qwglGetProcAddress("glAttachShader");
	qglCompileShader = (PFNGLCOMPILESHADERPROC)qwglGetProcAddress("glCompileShader");
	qglCreateProgram = (PFNGLCREATEPROGRAMPROC)qwglGetProcAddress("glCreateProgram");
	qglCreateShader = (PFNGLCREATESHADERPROC)qwglGetProcAddress("glCreateShader");
	qglDeleteProgram = (PFNGLDELETEPROGRAMPROC)qwglGetProcAddress("glDeleteProgram");
	qglDeleteShader = (PFNGLDELETESHADERPROC)qwglGetProcAddress("glDeleteShader");
	qglDetachShader = (PFNGLDETACHSHADERPROC)qwglGetProcAddress("glDetachShader");
	qglDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)qwglGetProcAddress("glDisableVertexAttribArray");
	qglEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)qwglGetProcAddress("glEnableVertexAttribArray");
	qglGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)qwglGetProcAddress("glGetAttribLocation");
	qglGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)qwglGetProcAddress("glGetProgramInfoLog");
	qglGetProgramiv = (PFNGLGETPROGRAMIVPROC)qwglGetProcAddress("glGetProgramiv");
	qglGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)qwglGetProcAddress("glGetShaderInfoLog");
	qglGetShaderiv = (PFNGLGETSHADERIVPROC)qwglGetProcAddress("glGetShaderiv");
	qglGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)qwglGetProcAddress("glGetUniformLocation");
	qglLinkProgram = (PFNGLLINKPROGRAMPROC)qwglGetProcAddress("glLinkProgram");
	qglShaderSource = (PFNGLSHADERSOURCEPROC)qwglGetProcAddress("glShaderSource");
	qglUniform1i = (PFNGLUNIFORM1IPROC)qwglGetProcAddress("glUniform1i");
	qglUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)qwglGetProcAddress("glUniformMatrix4fv");
	qglUseProgram = (PFNGLUSEPROGRAMPROC)qwglGetProcAddress("glUseProgram");
	qglVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)qwglGetProcAddress("glVertexAttribPointer");
}
