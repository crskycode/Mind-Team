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
#include "hud.h"
#include "ClientState.h"
#include "camera.h"
#include "kbutton.h"
#include "view.h"

#include <in_buttons.h>
#include <pm_defs.h>

#include <keydefs.h>
#include "GameUI_Interface.h"
#include "vgui_gamemenudialog.h"
#include "vgui_buysetupdialog.h"
#include "vgui_chatboxdialog.h"

extern "C" 
{
	DLLExport kbutton_t *KB_Find(const char *name);
	DLLExport void CL_CreateMove(float frametime, struct usercmd_s *cmd, int active);
	DLLExport int HUD_Key_Event(int eventcode, int keynum, const char *pszCurrentBinding);
}

// inputw32.cpp
void IN_Init(void);
void IN_Move(float frametime, usercmd_t *cmd);
void IN_Shutdown(void);

// view.cpp
void V_Init(void);

int	in_impulse = 0;

cvar_t	*cl_forwardspeed;
cvar_t	*cl_backspeed;
cvar_t	*cl_sidespeed;
cvar_t	*cl_movespeedkey;
/*
===============================================================================

KEY BUTTONS

Continuous button event tracking is complicated by the fact that two different
input sources (say, mouse button 1 and the control key) can both press the
same button, but the button should only be released when both of the
pressing key have been released.

When a key event issues a button command (+forward, +attack, etc), it appends
its key number as a parameter to the command so it can be matched up with
the release.

state bit 0 is the current state of the key
state bit 1 is edge triggered on the up to down transition
state bit 2 is edge triggered on the down to up transition

===============================================================================
*/


// Network
kbutton_t	in_forward;
kbutton_t	in_back;
kbutton_t	in_moveleft;
kbutton_t	in_moveright;
kbutton_t	in_speed;
kbutton_t	in_use;
kbutton_t	in_jump;
kbutton_t	in_attack;
kbutton_t	in_attack2;
kbutton_t	in_duck;
kbutton_t	in_reload;
kbutton_t	in_score;


// Fill
kbutton_t in_lookup;
kbutton_t in_lookdown;
kbutton_t in_moveup;
kbutton_t in_movedown;
kbutton_t in_left;
kbutton_t in_right;
kbutton_t in_strafe;


typedef struct kblist_s
{
	kblist_s*	next;
	kbutton_t*	pkey;
	char		name[32];

} kblist_t;

kblist_t *g_kbkeys = NULL;


DLLExport kbutton_t *KB_Find(const char *name)
{
	kblist_t *p = g_kbkeys;

	while (p)
	{
		if (!stricmp(name, p->name))
			return p->pkey;

		p = p->next;
	}

	return NULL;
}

void KB_Add(const char *name, kbutton_t *pkb)
{
	kblist_t	*p;
	kbutton_t	*kb;

	kb = KB_Find(name);
	
	if (kb)
		return;

	p = new kblist_t;
	memset(p, 0, sizeof(kblist_t));

	strcpy(p->name, name);
	p->pkey = pkb;

	p->next = g_kbkeys;
	g_kbkeys = p;
}

void KB_Init(void)
{
	g_kbkeys = NULL;

	//KB_Add("in_graph", &in_graph);
	//KB_Add("in_mlook", &in_mlook);
	//KB_Add("in_jlook", &in_jlook);
}

void KB_Shutdown(void)
{
	kblist_t *p = g_kbkeys;

	while (p)
	{
		kblist_t *n = p->next;
		delete p;
		p = n;
	}

	g_kbkeys = NULL;
}

void KeyDown(kbutton_t *b)
{
	int		k;
	char	*c;

	c = gEngfuncs.Cmd_Argv(1);

	if (c[0])
		k = atoi(c);
	else
		k = -1;

	if (k == b->down[0] || k == b->down[1])
		return;
	
	if (!b->down[0])
		b->down[0] = k;
	else if (!b->down[1])
		b->down[1] = k;
	else
	{
		gEngfuncs.Con_DPrintf("Three keys down for a button '%c' '%c' '%c'!\n", b->down[0], b->down[1], c);
		return;
	}
	
	if (b->state & 1)
		return;

	b->state |= 1 + 2;
}

void KeyUp(kbutton_t *b)
{
	int		k;
	char	*c;
	
	c = gEngfuncs.Cmd_Argv(1);

	if (c[0])
		k = atoi(c);
	else
	{
		b->down[0] = b->down[1] = 0;
		b->state = 4;
		return;
	}

	if (b->down[0] == k)
		b->down[0] = 0;
	else if (b->down[1] == k)
		b->down[1] = 0;
	else
		return;

	if (b->down[0] || b->down[1])
		return;

	if (!(b->state & 1))
		return;

	b->state &= ~1;
	b->state |= 4;
}

DLLExport int HUD_Key_Event(int down, int keynum, const char *pszCurrentBinding)
{
	if (down && keynum == K_ESCAPE)
	{
		if (ChatBoxDialog_IsVisible())
		{
			ChatBoxDialog_Close();
			return 0;
		}

		if (BuySetupDialog_IsVisible())
		{
			BuySetupDialog_Close();
			return 0;
		}

		if (gHUD.m_Radio.IsVisable())
		{
			gHUD.m_Radio.Hide();
			return 0;
		}

		//if (!GameMenuDialog_IsVisible())
		//{
		//	GameMenuDialog_Activate();
		//	return 0;
		//}
		//else
		//{
		//	GameMenuDialog_Close();
		//	return 0;
		//}
	}

	//if (down && keynum == K_F12)
	//{
	//	if (!GameMenuDialog_IsVisible())
	//	{
	//		GameMenuDialog_Activate();
	//		return 0;
	//	}
	//	else
	//	{
	//		GameMenuDialog_Close();
	//		return 0;
	//	}
	//}

	if (down && gHUD.m_Radio.IsVisable())
	{
		switch (keynum)
		{
			case '1':
			{
				gHUD.m_Radio.Select(1);
				return 0;
			}
			case '2':
			{
				gHUD.m_Radio.Select(2);
				return 0;
			}
			case '3':
			{
				gHUD.m_Radio.Select(3);
				return 0;
			}
			case '4':
			{
				gHUD.m_Radio.Select(4);
				return 0;
			}
			case '5':
			{
				gHUD.m_Radio.Select(5);
				return 0;
			}
			case '6':
			{
				gHUD.m_Radio.Hide();
				return 0;
			}
		}
	}

	const char *level = gEngfuncs.pfnGetLevelName();

	if ( *level )
	{
		if (down)
		{
			if (keynum == K_PGUP)
			{
				gHUD.m_Sensity.Inc();
			}
			else if (keynum == K_PGDN)
			{
				gHUD.m_Sensity.Dec();
			}
		}
	}

	return 1;
}


#define HANDLE_KEY_DOWN(x, y) void __##x(void) { KeyDown(&##y); }
#define HANDLE_KEY_UP(x, y) void __##x(void) { KeyUp(&##y); }

#define HOOK_KEY(x, y) gEngfuncs.pfnAddCommand(##x, __##y)


HANDLE_KEY_DOWN(IN_ForwardDown, in_forward);
HANDLE_KEY_UP(IN_ForwardUp, in_forward);
HANDLE_KEY_DOWN(IN_BackDown, in_back);
HANDLE_KEY_UP(IN_BackUp, in_back);
HANDLE_KEY_DOWN(IN_MoveLeftDown, in_moveleft);
HANDLE_KEY_UP(IN_MoveLeftUp, in_moveleft);
HANDLE_KEY_DOWN(IN_MoveRightDown, in_moveright);
HANDLE_KEY_UP(IN_MoveRightUp, in_moveright);
HANDLE_KEY_DOWN(IN_SpeedDown, in_speed);
HANDLE_KEY_UP(IN_SpeedUp, in_speed);
HANDLE_KEY_DOWN(IN_AttackDown, in_attack);
HANDLE_KEY_UP(IN_AttackUp, in_attack);
HANDLE_KEY_DOWN(IN_Attack2Down, in_attack2);
HANDLE_KEY_UP(IN_Attack2Up, in_attack2);
HANDLE_KEY_DOWN(IN_UseDown, in_use);
HANDLE_KEY_UP(IN_UseUp, in_use);
HANDLE_KEY_DOWN(IN_JumpDown, in_jump);
HANDLE_KEY_UP(IN_JumpUp, in_jump);
HANDLE_KEY_DOWN(IN_DuckDown, in_duck);
HANDLE_KEY_UP(IN_DuckUp, in_duck);
HANDLE_KEY_DOWN(IN_ReloadDown, in_reload);
HANDLE_KEY_UP(IN_ReloadUp, in_reload);

// Fill
HANDLE_KEY_DOWN(IN_LookupDown, in_lookup);
HANDLE_KEY_UP(IN_LookupUp, in_lookup);
HANDLE_KEY_DOWN(IN_LookdownDown, in_lookdown);
HANDLE_KEY_UP(IN_LookdownUp, in_lookdown);
HANDLE_KEY_DOWN(IN_MoveupDown, in_moveup);
HANDLE_KEY_UP(IN_MoveupUp, in_moveup);
HANDLE_KEY_DOWN(IN_MovedownDown, in_movedown);
HANDLE_KEY_UP(IN_MovedownUp, in_movedown);
HANDLE_KEY_DOWN(IN_LeftDown, in_left);
HANDLE_KEY_UP(IN_LeftUp, in_left);
HANDLE_KEY_DOWN(IN_RightDown, in_right);
HANDLE_KEY_UP(IN_RightUp, in_right);
HANDLE_KEY_DOWN(IN_StrafeDown, in_strafe);
HANDLE_KEY_DOWN(IN_StrafeUp, in_strafe);

void IN_Impulse(void)
{
	in_impulse = atoi( gEngfuncs.Cmd_Argv(1) );
}

void __IN_ScoreDown(void)
{
	KeyDown(&in_score);
	gHUD.m_ScoreBoard.Show();
}

void __IN_ScoreUp(void)
{
	KeyUp(&in_score);
	gHUD.m_ScoreBoard.Hide();
}

void __IN_TacMapDown(void)
{
	gHUD.m_TacticalMap.Show();
}

void __IN_TacMapUp(void)
{
	gHUD.m_TacticalMap.Hide();
}

void __IN_RadarDown(void)
{
	if (!gHUD.m_Radar.IsVisible())
		gHUD.m_Radar.Show();
	else
		gHUD.m_Radar.Hide();
}

void __IN_MissionDown(void)
{
	if (!gHUD.m_SpecMission.IsVisible())
		gHUD.m_SpecMission.Show();
	else
		gHUD.m_SpecMission.Hide();
}

void __IN_TeamMateDown(void)
{
	gHUD.m_Teammate.Toggle();
}

float CL_KeyState(kbutton_t &key)
{
	float	val = 0.0;
	int		impulsedown, impulseup, down;
	
	impulsedown = key.state & 2;
	impulseup	= key.state & 4;
	down		= key.state & 1;
	
	if (impulsedown && !impulseup)
	{
		val = down ? 0.5 : 0.0;
	}

	if (impulseup && !impulsedown)
	{
		val = down ? 0.0 : 0.0;
	}

	if (!impulsedown && !impulseup)
	{
		val = down ? 1.0 : 0.0;
	}

	if (impulsedown && impulseup)
	{
		if (down)
		{
			val = 0.75;
		}
		else
		{
			val = 0.25;
		}
	}

	key.state &= 1;

	return val;
}

int CL_ButtonBits(bool bResetState)
{
	int bits = 0;

	if (in_attack.state & 3)
	{
		bits |= IN_ATTACK;
	}
	
	if (in_duck.state & 3)
	{
		bits |= IN_DUCK;
	}
 
	if (in_jump.state & 3)
	{
		bits |= IN_JUMP;
	}

	if (in_forward.state & 3)
	{
		bits |= IN_FORWARD;
	}
	
	if (in_back.state & 3)
	{
		bits |= IN_BACK;
	}

	if (in_use.state & 3)
	{
		bits |= IN_USE;
	}
	
	if (in_moveleft.state & 3)
	{
		bits |= IN_MOVELEFT;
	}
	
	if (in_moveright.state & 3)
	{
		bits |= IN_MOVERIGHT;
	}

	if (in_attack2.state & 3)
	{
		bits |= IN_ATTACK2;
	}

	if (in_reload.state & 3)
	{
		bits |= IN_RELOAD;
	}

	if (in_score.state & 3)
	{
		bits |= IN_SCORE;
	}

	if (in_speed.state & 3)
	{
		bits |= IN_SPEED;
	}

	if (bResetState)
	{
		in_attack.state &= ~2;
		in_duck.state &= ~2;
		in_jump.state &= ~2;
		in_forward.state &= ~2;
		in_back.state &= ~2;
		in_use.state &= ~2;
		in_moveleft.state &= ~2;
		in_moveright.state &= ~2;
		in_attack2.state &= ~2;
		in_reload.state &= ~2;
		in_score.state &= ~2;
		in_speed.state &= ~2;
	}

	return bits;
}

void CL_ResetButtonBits(int bits)
{
	int bitsNew = CL_ButtonBits(0) ^ bits;

	// Has the attack button been changed
	if (bitsNew & IN_ATTACK)
	{
		// Was it pressed? or let go?
		if (bits & IN_ATTACK)
		{
			KeyDown(&in_attack);
		}
		else
		{
			// totally clear state
			in_attack.state &= ~7;
		}
	}
}

DLLExport void CL_CreateMove(float frametime, struct usercmd_s *cmd, int active)
{	
	float			spd;
	vec3_t			viewangles;
	static vec3_t	oldangles;

	if (active)
	{
		gEngfuncs.GetViewAngles(viewangles);

		memset(cmd, 0, sizeof(usercmd_t));

		gEngfuncs.SetViewAngles(viewangles);

		cmd->sidemove += cl_sidespeed->value * CL_KeyState(in_moveright);
		cmd->sidemove -= cl_sidespeed->value * CL_KeyState(in_moveleft);

		cmd->forwardmove += cl_forwardspeed->value * CL_KeyState(in_forward);
		cmd->forwardmove -= cl_backspeed->value * CL_KeyState(in_back);

		if (in_speed.state & 1)
		{
			cmd->forwardmove *= cl_movespeedkey->value;
			cmd->sidemove *= cl_movespeedkey->value;
			cmd->upmove *= cl_movespeedkey->value;
		}

		spd = gEngfuncs.GetClientMaxspeed();

		if (spd != 0.0)
		{
			float fmov = sqrt((cmd->forwardmove*cmd->forwardmove) + (cmd->sidemove*cmd->sidemove) + (cmd->upmove*cmd->upmove));

			if (fmov > spd)
			{
				float fratio = spd / fmov;
				cmd->forwardmove *= fratio;
				cmd->sidemove *= fratio;
				cmd->upmove *= fratio;
			}
		}

		IN_Move(frametime, cmd);
	}

	cmd->impulse = in_impulse;
	in_impulse = 0;

	cmd->buttons = CL_ButtonBits(true);

	gEngfuncs.GetViewAngles(viewangles);

	if (ClientState.bIsAlive)
	{
		VectorCopy(viewangles, cmd->viewangles);
		VectorCopy(viewangles, oldangles);
	}
	else
	{
		VectorCopy(oldangles, cmd->viewangles);
	}
}

void InitInput (void)
{
	HOOK_KEY("+forward", IN_ForwardDown);
	HOOK_KEY("-forward", IN_ForwardUp);
	HOOK_KEY("+back", IN_BackDown);
	HOOK_KEY("-back", IN_BackUp);
	HOOK_KEY("+moveleft", IN_MoveLeftDown);
	HOOK_KEY("-moveleft", IN_MoveLeftUp);
	HOOK_KEY("+moveright", IN_MoveRightDown);
	HOOK_KEY("-moveright", IN_MoveRightUp);
	HOOK_KEY("+speed", IN_SpeedDown);
	HOOK_KEY("-speed", IN_SpeedUp);
	HOOK_KEY("+attack", IN_AttackDown);
	HOOK_KEY("-attack", IN_AttackUp);
	HOOK_KEY("+attack2", IN_Attack2Down);
	HOOK_KEY("-attack2", IN_Attack2Up);
	HOOK_KEY("+use", IN_UseDown);
	HOOK_KEY("-use", IN_UseUp);
	HOOK_KEY("+jump", IN_JumpDown);
	HOOK_KEY("-jump", IN_JumpUp);
	HOOK_KEY("+duck", IN_DuckDown);
	HOOK_KEY("-duck", IN_DuckUp);
	HOOK_KEY("+reload", IN_ReloadDown);
	HOOK_KEY("-reload", IN_ReloadUp);
	HOOK_KEY("+score", IN_ScoreDown);
	HOOK_KEY("-score", IN_ScoreUp);
	HOOK_KEY("+showscores", IN_ScoreDown);
	HOOK_KEY("-showscores", IN_ScoreUp);
	HOOK_KEY("+tacmap", IN_TacMapDown);
	HOOK_KEY("-tacmap", IN_TacMapUp);

	// Only down
	HOOK_KEY("radar", IN_RadarDown);
	HOOK_KEY("mission", IN_MissionDown);
	HOOK_KEY("teammate", IN_TeamMateDown);

	// Fill
	HOOK_KEY("+lookup", IN_LookupDown);
	HOOK_KEY("-lookup", IN_LookupUp);
	HOOK_KEY("+lookdown", IN_LookdownDown);
	HOOK_KEY("-lookdown", IN_LookdownUp);
	HOOK_KEY("+moveup", IN_MoveupDown);
	HOOK_KEY("-moveup", IN_MoveupUp);
	HOOK_KEY("+movedown", IN_MovedownDown);
	HOOK_KEY("-movedown", IN_MovedownUp);
	HOOK_KEY("+left", IN_LeftDown);
	HOOK_KEY("-left", IN_LeftUp);
	HOOK_KEY("+right", IN_RightDown);
	HOOK_KEY("-right", IN_RightUp);
	HOOK_KEY("+strafe", IN_StrafeDown);
	HOOK_KEY("-strafe", IN_StrafeUp);

	//gEngfuncs.pfnAddCommand("impulse", IN_Impulse);

	cl_forwardspeed		= gEngfuncs.pfnRegisterVariable("cl_forwardspeed", "400", FCVAR_ARCHIVE);
	cl_backspeed		= gEngfuncs.pfnRegisterVariable("cl_backspeed", "400", FCVAR_ARCHIVE);
	cl_sidespeed		= gEngfuncs.pfnRegisterVariable("cl_sidespeed", "400", FCVAR_ARCHIVE);
	cl_movespeedkey		= gEngfuncs.pfnRegisterVariable("cl_movespeedkey", "0.3", FCVAR_ARCHIVE);

	// Initialize third person camera controls.
	CAM_Init();
	// Initialize inputs
	IN_Init();
	// Initialize keyboard
	KB_Init();
	// Initialize view system
	V_Init();
}

void ShutdownInput(void)
{
	IN_Shutdown();
	KB_Shutdown();
}