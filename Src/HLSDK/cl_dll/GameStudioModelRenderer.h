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

#ifndef GAMESTUDIOMODELRENDERER_H
#define GAMESTUDIOMODELRENDERER_H

#include "StudioModelRenderer.h"

class CGameStudioModelRenderer : public CStudioModelRenderer
{
public:
	void StudioSetupBones( void );
	int StudioDrawModel( int flags );
	int StudioDrawPlayer( int flags, struct entity_state_s *pplayer );
	void StudioProcessGait( entity_state_t *pplayer );
};

#endif