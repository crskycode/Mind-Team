/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
****/
// updates:
// 1-4-98	fixed initialization

#include <stdio.h>

#include <windows.h>

#include <gl\gl.h>
#include <gl\glu.h>
#include "glut.h"
#include "glext.h"

#include "mathlib.h"
#include "..\..\engine\studio.h"
#include "mdlviewer.h"


#pragma warning( disable : 4244 ) // conversion from 'double ' to 'float ', possible loss of data
#pragma warning( disable : 4305 ) // truncation from 'const double ' to 'float '

vec3_t		g_vright;		// needs to be set to viewer's right in order for chrome to work
float		g_lambert = 1.5;

float		gldepthmin = 0;
float		gldepthmax = 10.0;


/*
=============
R_Clear
=============
*/
void R_Clear (void)
{
	glDepthFunc (GL_LEQUAL);
	glDepthRange (gldepthmin, gldepthmax);
	glDepthMask( 1 );
}

static StudioModel tempmodel;

void mdlviewer_display( )
{
	R_Clear( );

	tempmodel.SetBlending( 0, 0.0 );
	tempmodel.SetBlending( 1, 0.0 );

	static float prev;
	float curr = GetTickCount( ) / 1000.0;
	tempmodel.AdvanceFrame( curr - prev );
	prev = curr;

	tempmodel.DrawModel( );
}

extern "C"
{
extern void (APIENTRY *glDeleteObjectARB)(GLhandleARB obj);
extern GLhandleARB (APIENTRY *glGetHandleARB)(GLenum pname);
extern void (APIENTRY *glDetachObjectARB)(GLhandleARB containerObj, GLhandleARB attachedObj);
extern GLhandleARB (APIENTRY *glCreateShaderObjectARB)(GLenum shaderType);
extern void (APIENTRY *glShaderSourceARB)(GLhandleARB shaderObj, GLsizei count, const GLcharARB **string, const GLint *length);
extern void (APIENTRY *glCompileShaderARB)(GLhandleARB shaderObj);
extern GLhandleARB (APIENTRY *glCreateProgramObjectARB)(void);
extern void (APIENTRY *glAttachObjectARB)(GLhandleARB containerObj, GLhandleARB obj);
extern void (APIENTRY *glLinkProgramARB)(GLhandleARB programObj);
extern void (APIENTRY *glUseProgramObjectARB)(GLhandleARB programObj);
extern void (APIENTRY *glValidateProgramARB)(GLhandleARB programObj);
extern void (APIENTRY *glUniform1fARB)(GLint location, GLfloat v0);
extern void (APIENTRY *glUniform2fARB)(GLint location, GLfloat v0, GLfloat v1);
extern void (APIENTRY *glUniform3fARB)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
extern void (APIENTRY *glUniform4fARB)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
extern void (APIENTRY *glUniform1iARB)(GLint location, GLint v0);
extern void (APIENTRY *glUniform2iARB)(GLint location, GLint v0, GLint v1);
extern void (APIENTRY *glUniform3iARB)(GLint location, GLint v0, GLint v1, GLint v2);
extern void (APIENTRY *glUniform4iARB)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
extern void (APIENTRY *glUniform1fvARB)(GLint location, GLsizei count, const GLfloat *value);
extern void (APIENTRY *glUniform2fvARB)(GLint location, GLsizei count, const GLfloat *value);
extern void (APIENTRY *glUniform3fvARB)(GLint location, GLsizei count, const GLfloat *value);
extern void (APIENTRY *glUniform4fvARB)(GLint location, GLsizei count, const GLfloat *value);
extern void (APIENTRY *glUniform1ivARB)(GLint location, GLsizei count, const GLint *value);
extern void (APIENTRY *glUniform2ivARB)(GLint location, GLsizei count, const GLint *value);
extern void (APIENTRY *glUniform3ivARB)(GLint location, GLsizei count, const GLint *value);
extern void (APIENTRY *glUniform4ivARB)(GLint location, GLsizei count, const GLint *value);
extern void (APIENTRY *glUniformMatrix2fvARB)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
extern void (APIENTRY *glUniformMatrix3fvARB)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
extern void (APIENTRY *glUniformMatrix4fvARB)(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
extern void (APIENTRY *glGetObjectParameterfvARB)(GLhandleARB obj, GLenum pname, GLfloat *params);
extern void (APIENTRY *glGetObjectParameterivARB)(GLhandleARB obj, GLenum pname, GLint *params);
extern void (APIENTRY *glGetInfoLogARB)(GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog);
extern void (APIENTRY *glGetAttachedObjectsARB)(GLhandleARB containerObj, GLsizei maxCount, GLsizei *count, GLhandleARB *obj);
extern GLint (APIENTRY *glGetUniformLocationARB)(GLhandleARB programObj, const GLcharARB *name);
extern void (APIENTRY *glGetActiveUniformARB)(GLhandleARB programObj, GLuint index, GLsizei maxLength, GLsizei *length, GLint *size, GLenum *type, GLcharARB *name);
extern void (APIENTRY *glGetUniformfvARB)(GLhandleARB programObj, GLint location, GLfloat *params);
extern void (APIENTRY *glGetUniformivARB)(GLhandleARB programObj, GLint location, GLint *params);
extern void (APIENTRY *glGetShaderSourceARB)(GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *source);

extern void (APIENTRY *glActiveTextureARB)(GLenum texture);

extern void (APIENTRY *glVertexAttrib3fARB)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
extern GLint (APIENTRY *glGetAttribLocationARB)(GLhandleARB programObj, const GLcharARB *name);
}

int LoadShader(char *vertexShader, char *fragmentShader);
bool LoadTGA(const char *filename, unsigned char *buffer, int bufferSize, int *wide, int *tall);

GLuint R_LoadTexture(const char *filename)
{
	GLuint textureID;
	static byte textureData[1024 * 1024 * 4];
	int	wide, tall;

	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	if ( !LoadTGA(filename, textureData, sizeof(textureData), &wide, &tall) )
		return 0;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureID;
}

GLuint LoadCubeMap()
{
	GLuint textureID;
	static byte textureData[1024 * 1024 * 4];
	int	wide, tall;

	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	LoadTGA(".\\sky\\right.tga", textureData, sizeof(textureData), &wide, &tall);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	LoadTGA(".\\sky\\left.tga", textureData, sizeof(textureData), &wide, &tall);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	LoadTGA(".\\sky\\top.tga", textureData, sizeof(textureData), &wide, &tall);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	LoadTGA(".\\sky\\bottom.tga", textureData, sizeof(textureData), &wide, &tall);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	LoadTGA(".\\sky\\back.tga", textureData, sizeof(textureData), &wide, &tall);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	LoadTGA(".\\sky\\front.tga", textureData, sizeof(textureData), &wide, &tall);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

	//LoadTGA(".\\sky\\Gold_map.tga", textureData, sizeof(textureData), &wide, &tall);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	//LoadTGA(".\\sky\\Gold_map.tga", textureData, sizeof(textureData), &wide, &tall);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	//LoadTGA(".\\sky\\Gold_map.tga", textureData, sizeof(textureData), &wide, &tall);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	//LoadTGA(".\\sky\\Gold_map.tga", textureData, sizeof(textureData), &wide, &tall);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	//LoadTGA(".\\sky\\Gold_map.tga", textureData, sizeof(textureData), &wide, &tall);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
	//LoadTGA(".\\sky\\Gold_map.tga", textureData, sizeof(textureData), &wide, &tall);
	//glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, wide, tall, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}


int g_StudioShaderProgram;
//int g_StudioCubeMap;
int g_StudioDiffuseMap;
int g_StudioSpecularMap;
int g_StudioEnvCubeMap;
int g_StudioNormalMap;

void mdlviewer_init( char *modelname )
{
	glutSetWindowTitle( modelname );

	// make a bogus texture
	// R_InitTexture( );
	g_StudioShaderProgram = LoadShader(".\\shader\\studio.vsh", ".\\shader\\studio.fsh");

	// load textures
	g_StudioDiffuseMap = R_LoadTexture("W:\\BACKUP\\Desktop\\FFF\\PV-AK47_Knife_UltimateGold\\PV-AK47_Knife_UltimateGold.TGA");
	g_StudioSpecularMap = R_LoadTexture("W:\\BACKUP\\Desktop\\FFF\\PV-AK47_Knife_UltimateGold\\AK47_Knife_UltimateGold_S.TGA");
	g_StudioEnvCubeMap = 0;
	g_StudioNormalMap = R_LoadTexture("W:\\BACKUP\\Desktop\\FFF\\PV-AK47_Knife_UltimateGold\\AK47_Knife_UltimateGold_N.TGA");

	//g_StudioDiffuseMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\FFF\\PV-AK-47-Beast\\PV_AK47-Beast.TGA");
	//g_StudioSpecularMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\FFF\\PV-AK-47-Beast\\AK47-Beast_S.TGA");
	//g_StudioEnvCubeMap = LoadCubeMap();
	//g_StudioNormalMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\FFF\\PV-AK-47-Beast\\AK47-Beast_N.TGA");

	//g_StudioDiffuseMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\FFF\\PV-M4A1_S_Transformers\\M4A1_S_Transformers_S.TGA");
	//g_StudioSpecularMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\FFF\\PV-M4A1_S_Transformers\\M4A1_S_Transformers_S.TGA");
	//g_StudioEnvCubeMap = 0;
	//g_StudioNormalMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\FFF\\PV-M4A1_S_Transformers\\M4A1_S_Transformers_N.TGA");

	//g_StudioDiffuseMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-AK12_K_IronSpider\\PV-AK12_K_IronSpider.tga");
	//g_StudioSpecularMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-AK12_K_IronSpider\\AK12_K_IronSpider_S.tga");
	//g_StudioEnvCubeMap = 0;
	//g_StudioNormalMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-AK12_K_IronSpider\\AK12_K_IronSpider_N.tga");

	//g_StudioDiffuseMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-M82A1_RoyalDragon4\\PV-M82A1_RoyalDragon4.TGA");
	//g_StudioSpecularMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-M82A1_RoyalDragon4\\M82A1_RoyalDragon4_S.TGA");
	//g_StudioEnvCubeMap = 0;
	//g_StudioNormalMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-M82A1_RoyalDragon4\\M82A1_RoyalDragon4_N.TGA");

	//g_StudioDiffuseMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-M4A1_SILENCER\\PV-M4A1-s-silver.TGA");
	//g_StudioSpecularMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-M4A1_SILENCER\\M4A1-s-silver_S.TGA");
	//g_StudioEnvCubeMap = 0;
	//g_StudioNormalMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-M4A1_SILENCER\\M4A1-s-silver_N.TGA");

	//g_StudioDiffuseMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-KUKRI_BEAST\\PV-Kukri_Beast.TGA");
	//g_StudioSpecularMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-KUKRI_BEAST\\Kukri_Beast_s.TGA");
	//g_StudioEnvCubeMap = 0;
	//g_StudioNormalMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-KUKRI_BEAST\\Kukri_Beast_n.TGA");

	//g_StudioDiffuseMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-M4A1_S_BornBeast\\PV-M4A1_S_BornBeast.TGA");
	//g_StudioSpecularMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-M4A1_S_BornBeast\\M4A1_S_BornBeast_S.TGA");
	//g_StudioEnvCubeMap = 0;
	//g_StudioNormalMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-M4A1_S_BornBeast\\M4A1_S_BornBeast_N.TGA");

	//g_StudioDiffuseMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\PV-Smoke_Phoenix.tga");
	//g_StudioSpecularMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\Smoke_Phoenix_S.tga");
	//g_StudioEnvCubeMap = 0;
	//g_StudioNormalMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\Smoke_Phoenix_N.tga");

	//g_StudioDiffuseMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\timg.tga");
	//g_StudioSpecularMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\timg_S.tga");
	//g_StudioEnvCubeMap = LoadCubeMap();
	//g_StudioNormalMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\timg_N.tga");

	//g_StudioDiffuseMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\wall\\wall.tga");
	//g_StudioSpecularMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\wall\\wall_S.tga");
	//g_StudioEnvCubeMap = 0;
	//g_StudioNormalMap = R_LoadTexture("C:\\Users\\Crsky\\Desktop\\wall\\wall_N.tga");


	tempmodel.Init( modelname );
	tempmodel.SetSequence( 0 );

	tempmodel.SetController( 0, 0.0 );
	tempmodel.SetController( 1, 0.0 );
	tempmodel.SetController( 2, 0.0 );
	tempmodel.SetController( 3, 0.0 );
	tempmodel.SetMouth( 0 );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.,1.,.1,10.);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor( 0, 0, 0.5, 0 );
}


void mdlviewer_nextsequence( void )
{
	int iSeq = tempmodel.GetSequence( );
	if (iSeq == tempmodel.SetSequence( iSeq + 1 ))
	{
		tempmodel.SetSequence( 0 );
	}
}


//////////////////////////////////////////////////////////////////


static int pstyle;
static int translate = 1;
static int mesh = 1;
float transx = 0, transy = 0, transz = -2, rotx=0, roty=-90;
static float amplitude = 0.03;
static float freq = 5.0f;
static float phase = .00003;
static int ox = -1, oy = -1;
static int show_t = 1;
static int mot;
#define PAN	1
#define ROT	2
#define ZOOM 3

void pan(int x, int y) 
{
    transx +=  (x-ox)/500.;
    transy -= (y-oy)/500.;
    ox = x; oy = y;
    glutPostRedisplay();
}

void zoom(int x, int y) 
{
    transz +=  (x-ox)/80.;
    ox = x;
    glutPostRedisplay();
}

void rotate(int x, int y) 
{
    rotx += x-ox;
    if (rotx > 360.) rotx -= 360.;
    else if (rotx < -360.) rotx += 360.;
    //roty += y-oy;
    //if (roty > 360.) roty -= 360.;
    //else if (roty < -360.) roty += 360.;
    ox = x; oy = y;
    glutPostRedisplay();
}

void motion(int x, int y) 
{
    if (mot == PAN) 
		pan(x, y);
    else if (mot == ROT) 
		rotate(x,y);
	else if ( mot == ZOOM )
		zoom( x, y );
}

void mouse(int button, int state, int x, int y) 
{
    if(state == GLUT_DOWN) {
	switch(button) {
	case GLUT_LEFT_BUTTON:
	    mot = PAN;
	    motion(ox = x, oy = y);
	    break;
	case GLUT_RIGHT_BUTTON:
		mot = ROT;
	    motion(ox = x, oy = y);
	    break;
	case GLUT_MIDDLE_BUTTON:
		mot = ZOOM;
		motion(ox = x, oy = y);
	    break;
	}
    } else if (state == GLUT_UP) {
	mot = 0;
    }
}

void help(void) 
{
    printf("left mouse     - pan\n");
    printf("right mouse    - rotate\n");
}

void init( char *arg ) 
{
	const char *extensions = (const char *)glGetString(GL_EXTENSIONS);

	if ( !strstr(extensions, "GL_ARB_gpu_shader5") )
	{
		exit( 1 );
	}

	*(PROC *)&glDeleteObjectARB = wglGetProcAddress("glDeleteObjectARB");
	*(PROC *)&glGetHandleARB = wglGetProcAddress("glGetHandleARB");
	*(PROC *)&glDetachObjectARB = wglGetProcAddress("glDetachObjectARB");
	*(PROC *)&glCreateShaderObjectARB = wglGetProcAddress("glCreateShaderObjectARB");
	*(PROC *)&glShaderSourceARB = wglGetProcAddress("glShaderSourceARB");
	*(PROC *)&glCompileShaderARB = wglGetProcAddress("glCompileShaderARB");
	*(PROC *)&glCreateProgramObjectARB = wglGetProcAddress("glCreateProgramObjectARB");
	*(PROC *)&glAttachObjectARB = wglGetProcAddress("glAttachObjectARB");
	*(PROC *)&glLinkProgramARB = wglGetProcAddress("glLinkProgramARB");
	*(PROC *)&glUseProgramObjectARB = wglGetProcAddress("glUseProgramObjectARB");
	*(PROC *)&glValidateProgramARB = wglGetProcAddress("glValidateProgramARB");
	*(PROC *)&glUniform1fARB = wglGetProcAddress("glUniform1fARB");
	*(PROC *)&glUniform2fARB = wglGetProcAddress("glUniform2fARB");
	*(PROC *)&glUniform3fARB = wglGetProcAddress("glUniform3fARB");
	*(PROC *)&glUniform4fARB = wglGetProcAddress("glUniform4fARB");
	*(PROC *)&glUniform1iARB = wglGetProcAddress("glUniform1iARB");
	*(PROC *)&glUniform2iARB = wglGetProcAddress("glUniform2iARB");
	*(PROC *)&glUniform3iARB = wglGetProcAddress("glUniform3iARB");
	*(PROC *)&glUniform4iARB = wglGetProcAddress("glUniform4iARB");
	*(PROC *)&glUniform1fvARB = wglGetProcAddress("glUniform1fvARB");
	*(PROC *)&glUniform2fvARB = wglGetProcAddress("glUniform2fvARB");
	*(PROC *)&glUniform3fvARB = wglGetProcAddress("glUniform3fvARB");
	*(PROC *)&glUniform4fvARB = wglGetProcAddress("glUniform4fvARB");
	*(PROC *)&glUniform1ivARB = wglGetProcAddress("glUniform1ivARB");
	*(PROC *)&glUniform2ivARB = wglGetProcAddress("glUniform2ivARB");
	*(PROC *)&glUniform3ivARB = wglGetProcAddress("glUniform3ivARB");
	*(PROC *)&glUniform4ivARB = wglGetProcAddress("glUniform4ivARB");
	*(PROC *)&glUniformMatrix2fvARB = wglGetProcAddress("glUniformMatrix2fvARB");
	*(PROC *)&glUniformMatrix3fvARB = wglGetProcAddress("glUniformMatrix3fvARB");
	*(PROC *)&glUniformMatrix4fvARB = wglGetProcAddress("glUniformMatrix4fvARB");
	*(PROC *)&glGetObjectParameterfvARB = wglGetProcAddress("glGetObjectParameterfvARB");
	*(PROC *)&glGetObjectParameterivARB = wglGetProcAddress("glGetObjectParameterivARB");
	*(PROC *)&glGetInfoLogARB = wglGetProcAddress("glGetInfoLogARB");
	*(PROC *)&glGetAttachedObjectsARB = wglGetProcAddress("glGetAttachedObjectsARB");
	*(PROC *)&glGetUniformLocationARB = wglGetProcAddress("glGetUniformLocationARB");
	*(PROC *)&glGetActiveUniformARB = wglGetProcAddress("glGetActiveUniformARB");
	*(PROC *)&glGetUniformfvARB = wglGetProcAddress("glGetUniformfvARB");
	*(PROC *)&glGetUniformivARB = wglGetProcAddress("glGetUniformivARB");
	*(PROC *)&glGetShaderSourceARB = wglGetProcAddress("glGetShaderSourceARB");

	*(PROC *)&glActiveTextureARB = wglGetProcAddress("glActiveTextureARB");

	*(PROC *)&glVertexAttrib3fARB = wglGetProcAddress("glVertexAttrib3fARB");
	*(PROC *)&glGetAttribLocationARB = wglGetProcAddress("glGetAttribLocationARB");

	mdlviewer_init("W:\\BACKUP\\Desktop\\FFF\\PV-AK47_Knife_UltimateGold\\PV-AK47_Knife_UltimateGold.mdl");
	//mdlviewer_init( "C:\\Users\\Crsky\\Desktop\\FFF\\PV-AK-47-Beast\\PV-AK-47-Beast.mdl" );
	//mdlviewer_init( "C:\\Users\\Crsky\\Desktop\\FFF\\PV-M4A1_S_Transformers\\PV-M4A1_S_Transformers.mdl" );
	//mdlviewer_init( "C:\\Users\\Crsky\\Desktop\\PV-AK12_K_IronSpider\\PV-AK12_K_IronSpider.mdl" );
	//mdlviewer_init( "C:\\Users\\Crsky\\Desktop\\PV-M82A1_RoyalDragon4\\PV-M82A1_RoyalDragon4.mdl" );
	//mdlviewer_init( "C:\\Users\\Crsky\\Desktop\\PV-M4A1_SILENCER\\PV-M4A1-s-silver.mdl" );
	//mdlviewer_init( "C:\\Users\\Crsky\\Desktop\\PV-KUKRI_BEAST\\PV-KUKRI_BEAST.mdl" );
	//mdlviewer_init( "C:\\Users\\Crsky\\Desktop\\PV-M4A1_S_BornBeast\\PV-M4A1_S_BornBeast.mdl" );
	//mdlviewer_init( "C:\\Users\\Crsky\\Desktop\\test.mdl" );
	//mdlviewer_init( "C:\\Users\\Crsky\\Desktop\\timg.mdl" );
	//mdlviewer_init ( "C:\\Users\\Crsky\\Desktop\\wall\\wall.mdl" );

    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 0.01, 10000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // glTranslatef(0.,0.,-5.5);
    // glTranslatef(-.2.,1.0,-1.5);

    glClearColor( 0, 0.5, 0.5, 0 );
}

vec3_t light_pos;

void display(void) 
{
	light_pos[0] = -0.20;
	light_pos[1] = 0.08;
	light_pos[2] = 0.00;

	glClearColor( 0, 0.0, 0.0, 0.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glPushMatrix();

    glTranslatef(transx, transy, transz);
    
	glDisable(GL_TEXTURE_2D);

	glLineWidth(2);
	//X
	glColor4ub(255, 0, 0, 255);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	glEnd();
	//Y
	glColor4ub(0, 255, 0, 255);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1, 0);
	glEnd();
	//Z
	glColor4ub(0, 0, 255, 255);
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);
	glEnd();

	//DrawLight
	glPointSize(8);
	glColor4f(1, 1, 1, 1);
	glBegin(GL_POINTS);
	glVertex3f( light_pos[0], light_pos[1], light_pos[2] );
	glEnd();

	glEnable(GL_TEXTURE_2D);

	glRotatef(rotx, 0., 1., 0.);
    glRotatef(roty, 1., 0., 0.);

    glScalef( 0.01, 0.01, 0.01 );
	glCullFace( GL_FRONT );
	glEnable( GL_DEPTH_TEST );

	mdlviewer_display( );

    glPopMatrix();
    glutSwapBuffers();

    glutPostRedisplay();
}

void reshape(int w, int h) 
{
    glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, (float)w / (float)h, 0.01, 10000.0);
	glMatrixMode(GL_MODELVIEW);
}

/*ARGSUSED1*/
void key(unsigned char key, int x, int y) 
{
    switch(key) 
	{
		case 'h': 
			help(); 
		break;

		case 'p':
			printf("Translation: %f, %f %f\n", transx, transy, transz );
		break;

		case '\033':	// Escape
			exit(EXIT_SUCCESS); 
		break;

		case ' ':
			mdlviewer_nextsequence( );
		break;

		default: 
		break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) 
{
	if (argc != 2)
	{
	//	printf("usage : %s <filename>\n", argv[0] );
	//	exit(1);
	}

    glutInitWindowSize(800, 800);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
    (void)glutCreateWindow(argv[0]);
    init( argv[1] );
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();
    return 0;
}

