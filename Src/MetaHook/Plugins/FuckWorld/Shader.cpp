#include <metahook.h>
#include "qgl.h"
#include "sys.h"

int CreateShader(const char *vertexShader, const char *fragmentShader)
{
	GLhandleARB		hProgram;
	GLcharARB *		szSource;
	GLuint			hShader;
	GLcharARB		szInfoLog[1024];
	GLint			iInfoLen;

	hProgram = qglCreateProgramObjectARB();

	if (vertexShader)
	{
		szSource = (GLcharARB *)gEngfuncs.COM_LoadFile((char *)vertexShader, 5, NULL);

		if (szSource)
		{
			hShader = qglCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);

			qglShaderSourceARB(hShader, 1, (const GLcharARB **)&szSource, NULL);
			qglCompileShaderARB(hShader);

			qglGetInfoLogARB(hShader, sizeof(szInfoLog), &iInfoLen, szInfoLog);

			gEngfuncs.COM_FreeFile(szSource);

			if (szInfoLog[0])
			{
				if (strstr(szInfoLog, "error") || strstr(szInfoLog, "Error"))
				{
					Sys_Error("Shader \"%s\" compile failed\n%s", vertexShader, szInfoLog);
					return 0;
				}
			}

			qglAttachObjectARB(hProgram, hShader);
		}
		else
		{
			Sys_Error("Shader %s load failed", vertexShader);
		}
	}

	if (fragmentShader)
	{
		szSource = (GLcharARB *)gEngfuncs.COM_LoadFile((char *)fragmentShader, 5, NULL);

		if (szSource)
		{
			hShader = qglCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

			qglShaderSourceARB(hShader, 1, (const GLcharARB **)&szSource, NULL);
			qglCompileShaderARB(hShader);

			qglGetInfoLogARB(hShader, sizeof(szInfoLog), &iInfoLen, szInfoLog);

			gEngfuncs.COM_FreeFile(szSource);

			if (szInfoLog[0])
			{
				if (strstr(szInfoLog, "error") || strstr(szInfoLog, "Error"))
				{
					Sys_Error("Shader \"%s\" compile failed\n%s", fragmentShader, szInfoLog);
					return 0;
				}
			}

			qglAttachObjectARB(hProgram, hShader);
		}
		else
		{
			Sys_Error("Shader %s load failed", fragmentShader);
		}
	}

	qglLinkProgramARB(hProgram);

	return hProgram;
}