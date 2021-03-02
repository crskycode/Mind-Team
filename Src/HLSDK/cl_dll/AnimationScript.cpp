/***
 *
 * Copyright (c) 1996-2002, Valve LLC. All rights reserved.
 *
 * This product contains software technology licensed from Id
 * Software, Inc. ("Id Technology"). Id Technology (c) 1996 Id Software, Inc.
 * All Rights Reserved.
 *
 * Use, distribution, and modification of this source code and/or resulting
 * object code is restricted to non-commercial enhancements to products from
 * Valve LLC. All other use, distribution, or modification is prohibited
 * without written permission from Valve LLC.
 *
***/

#include "cl_dll.h"
#include <newkeyvalues.h>
#include "cmdparse.h"
#include "AnimationScript.h"


#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

#define MAX_FRAME	40
#define MAX_LAYER	8
#define MAX_SCRIPT	8

struct AnimFrame
{
	int			m_Time;
	AnimData	m_Data;
};

struct AnimLayer
{
	char		m_Name[64];
	int			m_NumFrame;
	AnimFrame	m_Frame[MAX_FRAME];
};

struct AnimScript
{
	char		m_Name[64];
	int			m_NumLayer;
	AnimLayer	m_Layer[MAX_LAYER];
};

struct AnimStruct
{
	int			m_NumScript;
	AnimScript	m_Script[MAX_SCRIPT];
};

static AnimStruct gAnimStruct;

void AnimScript_Init(void)
{
	memset(&gAnimStruct, 0, sizeof(AnimStruct));
}

void AnimScript_LoadFile(const char *filename)
{
	char *	buffer;
	int		length;

	NewKeyValues *	pkvd;

	buffer = (char *)gEngfuncs.COM_LoadFile(filename, 5, &length);

	if (!buffer)
	{
		gEngfuncs.Con_Printf("Error: Couldn't to load animation script !\n");
		return;
	}

	buffer[length] = '\0';

	pkvd = new NewKeyValues("Script");

	if (!pkvd->LoadFromBuffer(buffer))
	{
		gEngfuncs.Con_Printf("Error: Couldn't to parse animation script !\n");
	}

	gEngfuncs.COM_FreeFile(buffer);


	if (gAnimStruct.m_NumScript >= MAX_SCRIPT)
	{
		Sys_Error("AnimScript_LoadFile: m_NumAnims >= MAX_SCRIPT");
	}

	int index = -1;

	for (int i = 0; i < gAnimStruct.m_NumScript; i++)
	{
		if (!strcmp(gAnimStruct.m_Script[i].m_Name, pkvd->GetName()))
		{
			index = i;
			break;
		}
	}

	if (index == -1)
	{
		index = gAnimStruct.m_NumScript++;
	}

	AnimScript &script = gAnimStruct.m_Script[index];

	memset(&script, 0, sizeof(AnimScript));

	strcpy(script.m_Name, pkvd->GetName());

	for (NewKeyValues *l = pkvd->GetFirstSubKey(); l; l = l->GetNextSubKey())
	{
		if (script.m_NumLayer >= MAX_LAYER)
		{
			Sys_Error("AnimScript_LoadFile: m_NumLayer >= MAX_LAYER");
		}

		AnimLayer &layer = script.m_Layer[script.m_NumLayer++];

		strcpy(layer.m_Name, l->GetName());

		for (NewKeyValues *f = l->GetFirstSubKey(); f; f = f->GetNextSubKey())
		{
			if (layer.m_NumFrame >= MAX_FRAME)
			{
				Sys_Error("AnimScript_LoadFile: m_NumFrames >= MAX_FRAME");
			}

			AnimFrame &frame = layer.m_Frame[layer.m_NumFrame++];

			frame.m_Time = atoi(f->GetName());

			Cmd_Init(f->GetString("origin"));
			frame.m_Data.vOrigin[0] = atof(Cmd_Argv(0));
			frame.m_Data.vOrigin[1] = atof(Cmd_Argv(1));
			frame.m_Data.vOrigin[2] = atof(Cmd_Argv(2));

			Cmd_Init(f->GetString("scale"));
			frame.m_Data.vScale[0] = atof(Cmd_Argv(0));
			frame.m_Data.vScale[1] = atof(Cmd_Argv(1));

			frame.m_Data.fAngle = f->GetFloat("angle");
			frame.m_Data.fAlpha = f->GetFloat("alpha");
			frame.m_Data.iTexture = f->GetInt("texture");
			frame.m_Data.bLastFrame = FALSE;
		}
	}

	pkvd->deleteThis();
}

int AnimScript_FindScript(const char *script)
{
	for (int i = 0; i < gAnimStruct.m_NumScript; i++)
	{
		if (!strcmp(gAnimStruct.m_Script[i].m_Name, script))
		{
			return i;
		}
	}

	return -1;
}

int AnimScript_FindLayout(int script, const char *layout)
{
	if (script < 0 || script >= gAnimStruct.m_NumScript)
	{
		return -1;
	}

	AnimScript &s = gAnimStruct.m_Script[script];

	for (int i = 0; i < s.m_NumLayer; i++)
	{
		if (!strcmp(s.m_Layer[i].m_Name, layout))
		{
			return i;
		}
	}

	return -1;
}

int AnimScript_GetAnimTime(int index, int layout)
{
	if (index < 0 || index >= gAnimStruct.m_NumScript)
	{
		return NULL;
	}

	AnimScript &s = gAnimStruct.m_Script[index];

	if (layout < 0 || layout >= s.m_NumLayer)
	{
		return NULL;
	}

	AnimLayer &l = s.m_Layer[layout];

	if (l.m_NumFrame == 0)
	{
		return NULL;
	}

	return l.m_Frame[l.m_NumFrame-1].m_Time;
}

AnimData *AnimScript_GetData(int index, int layout, int time)
{
	static AnimData result;

	if (index < 0 || index >= gAnimStruct.m_NumScript)
	{
		return NULL;
	}

	AnimScript &s = gAnimStruct.m_Script[index];

	if (layout < 0 || layout >= s.m_NumLayer)
	{
		return NULL;
	}

	AnimLayer &l = s.m_Layer[layout];

	if (l.m_NumFrame == 0)
	{
		return NULL;
	}

	time = clamp(time, l.m_Frame[0].m_Time, l.m_Frame[l.m_NumFrame-1].m_Time);

	memset(&result, 0, sizeof(AnimData));

	for (int i = 0; i < l.m_NumFrame; i++)
	{
		AnimFrame &frame = l.m_Frame[i];

		if (frame.m_Time == time)
		{
			memcpy(&result, &frame.m_Data, sizeof(AnimData));

			if (i == l.m_NumFrame -1)
			{
				result.bLastFrame = TRUE;
			}

			return &result;
		}

		if (i + 1 < l.m_NumFrame)
		{
			AnimFrame &nextFrame = l.m_Frame[i + 1];

			if (time > frame.m_Time && time < nextFrame.m_Time)
			{
				float percen = (float)(time - frame.m_Time) / (float)(nextFrame.m_Time - frame.m_Time);

				result.vOrigin[0] = frame.m_Data.vOrigin[0] + (nextFrame.m_Data.vOrigin[0] - frame.m_Data.vOrigin[0]) * percen;
				result.vOrigin[1] = frame.m_Data.vOrigin[1] + (nextFrame.m_Data.vOrigin[1] - frame.m_Data.vOrigin[1]) * percen;
				result.vOrigin[2] = frame.m_Data.vOrigin[2] + (nextFrame.m_Data.vOrigin[2] - frame.m_Data.vOrigin[2]) * percen;

				result.vScale[0] = frame.m_Data.vScale[0] + (nextFrame.m_Data.vScale[0] - frame.m_Data.vScale[0]) * percen;
				result.vScale[1] = frame.m_Data.vScale[1] + (nextFrame.m_Data.vScale[1] - frame.m_Data.vScale[1]) * percen;

				result.fAngle = frame.m_Data.fAngle + (nextFrame.m_Data.fAngle - frame.m_Data.fAngle) * percen;
				result.fAlpha = frame.m_Data.fAlpha + (nextFrame.m_Data.fAlpha - frame.m_Data.fAlpha) * percen;
				result.iTexture = frame.m_Data.iTexture;
				result.bLastFrame = FALSE;

				return &result;
			}
		}
	}

	return NULL;
}

AnimData *AnimScript_GetData(int script, int layout, float percen)
{
	if (script < 0 || script >= gAnimStruct.m_NumScript)
	{
		return NULL;
	}

	AnimScript &s = gAnimStruct.m_Script[script];

	if (layout < 0 || layout >= s.m_NumLayer)
	{
		return NULL;
	}

	AnimLayer &l = s.m_Layer[layout];

	if (l.m_NumFrame == 0)
	{
		return NULL;
	}

	percen = clamp(percen, 0, 1);

	AnimFrame &startFrame = l.m_Frame[0];
	AnimFrame &endFrame = l.m_Frame[l.m_NumFrame - 1];

	int time = (endFrame.m_Time - startFrame.m_Time) * percen;

	return AnimScript_GetData(script, layout, time);
}