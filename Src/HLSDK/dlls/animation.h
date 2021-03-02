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

#ifndef ANIMATION_H
#define ANIMATION_H

int LookupActivity( void *pmodel, entvars_t *pev, int activity );
int LookupSequence( void *pmodel, const char *label );
void GetSequenceInfo( void *pmodel, entvars_t *pev, float *pflFrameRate, float *pflGroundSpeed );
int GetSequenceFlags( void *pmodel, entvars_t *pev );
int LookupAnimationEvents( void *pmodel, entvars_t *pev, float flStart, float flEnd );
float SetController( void *pmodel, entvars_t *pev, int iController, float flValue );
float SetBlending( void *pmodel, entvars_t *pev, int iBlender, float flValue );
void SetBodygroup( void *pmodel, entvars_t *pev, int iGroup, int iValue );
int GetBodygroup( void *pmodel, entvars_t *pev, int iGroup );

#endif