/**
 * Dynamically link to the OpenGL library
 * 2017-11-20
**/

#ifndef QGL_H
#define QGL_H

#include <gl/gl.h>
#include "glext.h"

// OpenGL wgl prototypes
typedef PROC (WINAPI *PFNWGLGETPROCADDRESSPROC)(LPCSTR lpszProc);

/* GL_VERSION_1_0 */
typedef void (APIENTRY *PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (APIENTRY *PFNGLDEPTHMASKPROC)(GLboolean flag);
typedef void (APIENTRY *PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
typedef void (APIENTRY *PFNGLFRUSTUMPROC)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
typedef void (APIENTRY *PFNGLLOADIDENTITYPROC)(void);
typedef void (APIENTRY *PFNGLMATRIXMODEPROC)(GLenum mode);
typedef void (APIENTRY *PFNGLPOPMATRIXPROC)(void);
typedef void (APIENTRY *PFNGLPUSHMATRIXPROC)(void);
typedef void (APIENTRY *PFNGLREADPIXELSPROC)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
typedef void (APIENTRY *PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
typedef void (APIENTRY *PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (APIENTRY *PFNGLTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);

extern PFNGLBINDTEXTUREPROC qglBindTexture;
extern PFNGLDEPTHMASKPROC qglDepthMask;
extern PFNGLDRAWELEMENTSPROC qglDrawElements;
extern PFNGLFRUSTUMPROC qglFrustum;
extern PFNGLLOADIDENTITYPROC qglLoadIdentity;
extern PFNGLMATRIXMODEPROC qglMatrixMode;
extern PFNGLPOPMATRIXPROC qglPopMatrix;
extern PFNGLPUSHMATRIXPROC qglPushMatrix;
extern PFNGLREADPIXELSPROC qglReadPixels;
extern PFNGLTEXIMAGE2DPROC qglTexImage2D;
extern PFNGLTEXPARAMETERIPROC qglTexParameteri;
extern PFNGLTEXSUBIMAGE2DPROC qglTexSubImage2D;

/* GL_VERSION_1_5 */
extern PFNGLBINDBUFFERPROC qglBindBuffer;
extern PFNGLBUFFERDATAPROC qglBufferData;
extern PFNGLDELETEBUFFERSPROC qglDeleteBuffers;
extern PFNGLGENBUFFERSPROC qglGenBuffers;
extern PFNGLMAPBUFFERPROC qglMapBuffer;
extern PFNGLUNMAPBUFFERPROC qglUnmapBuffer;

/* GL_VERSION_2_0 */
extern PFNGLATTACHSHADERPROC qglAttachShader;
extern PFNGLCOMPILESHADERPROC qglCompileShader;
extern PFNGLCREATEPROGRAMPROC qglCreateProgram;
extern PFNGLCREATESHADERPROC qglCreateShader;
extern PFNGLDELETEPROGRAMPROC qglDeleteProgram;
extern PFNGLDELETESHADERPROC qglDeleteShader;
extern PFNGLDETACHSHADERPROC qglDetachShader;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC qglDisableVertexAttribArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC qglEnableVertexAttribArray;
extern PFNGLGETATTRIBLOCATIONPROC qglGetAttribLocation;
extern PFNGLGETPROGRAMINFOLOGPROC qglGetProgramInfoLog;
extern PFNGLGETPROGRAMIVPROC qglGetProgramiv;
extern PFNGLGETSHADERINFOLOGPROC qglGetShaderInfoLog;
extern PFNGLGETSHADERIVPROC qglGetShaderiv;
extern PFNGLGETUNIFORMLOCATIONPROC qglGetUniformLocation;
extern PFNGLLINKPROGRAMPROC qglLinkProgram;
extern PFNGLSHADERSOURCEPROC qglShaderSource;
extern PFNGLUNIFORM1IPROC qglUniform1i;
extern PFNGLUNIFORMMATRIX4FVPROC qglUniformMatrix4fv;
extern PFNGLUSEPROGRAMPROC qglUseProgram;
extern PFNGLVERTEXATTRIBPOINTERPROC qglVertexAttribPointer;


void QGL_Init(void);

#endif