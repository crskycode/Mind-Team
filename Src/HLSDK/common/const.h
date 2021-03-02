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

#ifndef CONST_H
#define CONST_H

#define FL_FLY				(1<<0)
#define FL_SWIM				(1<<1)
#define FL_CONVEYOR			(1<<2)
#define FL_CLIENT			(1<<3)
#define FL_INWATER			(1<<4)
#define FL_MONSTER			(1<<5)
#define FL_GODMODE			(1<<6)
#define FL_NOTARGET			(1<<7)
#define FL_SKIPLOCALHOST	(1<<8)
#define FL_ONGROUND			(1<<9)
#define FL_PARTIALGROUND	(1<<10)
#define FL_WATERJUMP		(1<<11)
#define FL_FROZEN			(1<<12)
#define FL_FAKECLIENT		(1<<13)
#define FL_DUCKING			(1<<14)
#define FL_FLOAT			(1<<15)
#define FL_GRAPHED			(1<<16)
//#define FL_IMMUNE_WATER		(1<<17)
//#define FL_IMMUNE_SLIME		(1<<18)
//#define FL_IMMUNE_LAVA		(1<<19)
#define FL_PROXY			(1<<20)
#define FL_ALWAYSTHINK		(1<<21)
#define FL_BASEVELOCITY		(1<<22)
#define FL_MONSTERCLIP		(1<<23)
#define FL_ONTRAIN			(1<<24)
#define FL_WORLDBRUSH		(1<<25)
#define FL_SPECTATOR		(1<<26)
#define FL_CUSTOMENTITY		(1<<29)
#define FL_KILLME			(1<<30)
#define FL_DORMANT			(1<<31)

#define FTRACE_SIMPLEBOX	(1<<0)

#define WALKMOVE_NORMAL		0
#define WALKMOVE_WORLDONLY	1
#define WALKMOVE_CHECKONLY	2

#define MOVETYPE_NONE			0
//#define MOVETYPE_ANGLENOCLIP	1
//#define MOVETYPE_ANGLECLIP		2
#define MOVETYPE_WALK			3		// Player only - moving on the ground
#define MOVETYPE_STEP			4		// gravity, special edge handling -- monsters use this
#define MOVETYPE_FLY			5		// No gravity, but still collides with stuff
#define MOVETYPE_TOSS			6		// gravity/collisions
#define MOVETYPE_PUSH			7		// no clip to world, push and crush
#define MOVETYPE_NOCLIP			8		// No gravity, no collisions, still do velocity/avelocity
#define MOVETYPE_FLYMISSILE		9		// extra size to monsters
#define MOVETYPE_BOUNCE			10		// Just like Toss, but reflect velocity when contacting surfaces
#define MOVETYPE_BOUNCEMISSILE	11		// bounce w/o gravity
#define MOVETYPE_FOLLOW			12		// track movement of aiment
#define MOVETYPE_PUSHSTEP		13		// BSP model that needs physics/world collisions (uses nearest hull for world collision)

#define SOLID_NOT		0		// no interaction with other objects
#define SOLID_TRIGGER	1		// touch on edge, but not blocking
#define SOLID_BBOX		2		// touch on edge, block
#define SOLID_SLIDEBOX	3		// touch on edge, but not an onground
#define SOLID_BSP		4		// bsp clip, touch on edge, block

#define DEAD_NO				0
#define DEAD_DYING			1
#define DEAD_DEAD			2
#define DEAD_RESPAWNABLE	3
#define DEAD_DISCARDBODY	4

#define DAMAGE_NO	0
#define DAMAGE_YES	1
#define DAMAGE_AIM	2

#define EF_BRIGHTFIELD	(1<<0)
#define EF_MUZZLEFLASH	(1<<1)
#define EF_BRIGHTLIGHT	(1<<2)
#define EF_DIMLIGHT		(1<<3)
#define EF_INVLIGHT		(1<<4)
#define EF_NOINTERP		(1<<5)
#define EF_LIGHT		(1<<6)
#define EF_NODRAW		(1<<7)

#define EFLAG_SLERP	1

#define TE_BEAMPOINTS		0
#define TE_BEAMENTPOINT		1
#define TE_GUNSHOT			2
#define TE_EXPLOSION		3
#define TE_TAREXPLOSION		4
#define TE_SMOKE			5
#define TE_TRACER			6
#define TE_LIGHTNING		7
#define TE_BEAMENTS			8
#define TE_SPARKS			9
#define TE_LAVASPLASH		10
#define TE_TELEPORT			11
#define TE_EXPLOSION2		12
#define TE_BSPDECAL			13
#define TE_IMPLOSION		14
#define TE_SPRITETRAIL		15
#define TE_BEAM				16
#define TE_SPRITE			17
#define TE_BEAMSPRITE		18
#define TE_BEAMTORUS		19
#define TE_BEAMDISK			20
#define TE_BEAMCYLINDER 	21
#define TE_BEAMFOLLOW		22
#define TE_GLOWSPRITE		23
#define TE_BEAMRING			24
#define TE_STREAK_SPLASH	25
#define TE_BEAMHOSE			26
#define TE_DLIGHT			27
#define TE_ELIGHT			28
#define TE_TEXTMESSAGE		29
#define TE_LINE				30
#define TE_BOX				31
#define TE_KILLBEAM			99
#define TE_LARGEFUNNEL		100
#define TE_BLOODSTREAM		101
#define TE_SHOWLINE			102
#define TE_BLOOD			103
#define TE_DECAL			104
#define TE_FIZZ				105
#define TE_MODEL			106
#define TE_EXPLODEMODEL		107
#define TE_BREAKMODEL		108
#define TE_GUNSHOTDECAL		109
#define TE_SPRITE_SPRAY		110
#define TE_ARMOR_RICOCHET	111
#define TE_PLAYERDECAL		112
#define TE_BUBBLES			113
#define TE_BUBBLETRAIL		114
#define TE_BLOODSPRITE		115
#define TE_WORLDDECAL		116
#define TE_WORLDDECALHIGH	117
#define TE_DECALHIGH		118
#define TE_PROJECTILE		119
#define TE_SPRAY			120
#define TE_PLAYERSPRITES	121
#define TE_PARTICLEBURST	122
#define TE_FIREFIELD		123
#define TE_PLAYERATTACHMENT			124
#define TE_KILLPLAYERATTACHMENTS	125
#define TE_MULTIGUNSHOT		126
#define TE_USERTRACER		127

#define TE_EXPLFLAG_NONE		(1<<0)
#define TE_EXPLFLAG_NOADDITIVE	(1<<1)
#define TE_EXPLFLAG_NODLIGHTS	(1<<2)
#define TE_EXPLFLAG_NOSOUND		(1<<3)
#define TE_EXPLFLAG_NOPARTICLES	(1<<4)

#define TEFIRE_FLAG_ALLFLOAT	(1<<1)
#define TEFIRE_FLAG_SOMEFLOAT	(1<<2)
#define TEFIRE_FLAG_LOOP		(1<<3)
#define TEFIRE_FLAG_ALPHA		(1<<4)
#define TEFIRE_FLAG_PLANAR		(1<<5)

#define MSG_BROADCAST		0
#define MSG_ONE				1
#define MSG_ALL				2
#define MSG_INIT			3
#define MSG_PVS				4
#define MSG_PAS				5
#define MSG_PVS_R			6
#define MSG_PAS_R			7
#define MSG_ONE_UNRELIABLE	8
#define MSG_SPEC			9

#define CONTENTS_EMPTY		-1
#define CONTENTS_SOLID		-2
#define CONTENTS_WATER		-3
#define CONTENTS_SLIME		-4
#define CONTENTS_LAVA		-5
#define CONTENTS_SKY		-6
//#define CONTENTS_ORIGIN		-7
//#define CONTENTS_CLIP		-8
//#define CONTENTS_CURRENT_0		-9
//#define CONTENTS_CURRENT_90		-10
//#define CONTENTS_CURRENT_180	-11
//#define CONTENTS_CURRENT_270	-12
//#define CONTENTS_CURRENT_UP		-13
//#define CONTENTS_CURRENT_DOWN	-14
//#define CONTENTS_TRANSLUCENT	-15
#define CONTENTS_LADDER		-16
#define CONTENT_FLYFIELD	-17
#define CONTENT_GRAVITY_FLYFIELD	-18
#define CONTENT_FOG			-19

#define CONTENT_EMPTY	CONTENTS_EMPTY
#define CONTENT_SOLID	CONTENTS_SOLID
#define CONTENT_WATER	CONTENTS_WATER
#define CONTENT_SLIME	CONTENTS_SLIME
#define CONTENT_LAVA	CONTENTS_LAVA
#define CONTENT_SKY		CONTENTS_SKY

#define CHAN_AUTO		0
#define CHAN_WEAPON		1
#define CHAN_VOICE		2
#define CHAN_ITEM		3
#define CHAN_BODY		4
#define CHAN_STREAM		5
#define CHAN_STATIC		6
#define CHAN_NETWORKVOICE_BASE	7
#define CHAN_NETWORKVOICE_END	500

#define ATTN_NONE	0.0
#define ATTN_NORM	0.8
#define ATTN_IDLE	2.0
#define ATTN_STATIC	1.25

#define PITCH_NORM	100
#define PITCH_LOW	95
#define PITCH_HIGH	120

#define VOL_NORM	1.0

#define PLAT_LOW_TRIGGER	1

#define SF_TRAIN_WAIT_RETRIGGER	(1<<1)
#define SF_TRAIN_START_ON		(1<<2)
#define SF_TRAIN_PASSABLE		(1<<3)

#define BREAK_TYPEMASK	0x4F
#define BREAK_GLASS		0x01
#define BREAK_METAL		0x02
#define BREAK_FLESH		0x04
#define BREAK_WOOD		0x08
#define BREAK_SMOKE		0x10
#define BREAK_TRANS		0x20
#define BREAK_CONCRETE	0x40
#define BREAK_2			0x80

#define BOUNCE_GLASS		BREAK_GLASS
#define BOUNCE_METAL		BREAK_METAL
#define BOUNCE_FLESH		BREAK_FLESH
#define BOUNCE_WOOD			BREAK_WOOD
#define BOUNCE_SHRAP		0x10
#define BOUNCE_SHELL		0x20
#define BOUNCE_CONCRETE		BREAK_CONCRETE
#define BOUNCE_SHOTSHELL	0x80

#define TE_BOUNCE_NULL		0
#define TE_BOUNCE_SHELL		1
#define TE_BOUNCE_SHOTSHELL	2

enum
{
	kRenderNormal,
	kRenderTransColor,
	kRenderTransTexture,
	kRenderGlow,
	kRenderTransAlpha,
	kRenderTransAdd,
};

enum
{
	kRenderFxNone,
	kRenderFxPulseSlow,
	kRenderFxPulseFast,
	kRenderFxPulseSlowWide,
	kRenderFxPulseFastWide,
	kRenderFxFadeSlow,
	kRenderFxFadeFast,
	kRenderFxSolidSlow,
	kRenderFxSolidFast,
	kRenderFxStrobeSlow,
	kRenderFxStrobeFast,
	kRenderFxStrobeFaster,
	kRenderFxFlickerSlow,
	kRenderFxFlickerFast,
	kRenderFxNoDissipation,
	kRenderFxDistort,
	kRenderFxHologram,
	kRenderFxDeadPlayer,
	kRenderFxExplode,
	kRenderFxGlowShell,
	kRenderFxClampMinScale,
};

typedef unsigned int string_t;
typedef unsigned int func_t;

typedef unsigned char byte;
typedef unsigned short word;

typedef int qboolean;

struct color24
{
	byte r, g, b;
};

struct colorVec
{
	unsigned r, g, b, a;
};

typedef struct link_s
{
	struct link_s *prev, *next;
	
} link_t;

#endif