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
#include "ClientState.h"
#include "camera.h"

#include <usercmd.h>
#include <ref_params.h>
#include <hltv.h>

extern "C"
{
	#include <pmtrace.h>
	#include <pm_hull.h>
	#include <pm_movevars.h>
	#include <pm_shared.h>

	int PM_GetPhysEntInfo( int ent );

	extern vec3_t vJumpOrigin;
	extern vec3_t vJumpAngles;
}

extern "C"
{
	DLLExport void V_CalcRefdef( ref_params_t *pparams );
}


extern cvar_t	*cl_forwardspeed;

vec3_t	v_origin, v_angles, v_cl_angles, v_sim_org, v_lastAngles;
float	v_frametime, v_lastDistance;

vec3_t	ev_punchangle;

cvar_t	*cl_vsmoothing;

cvar_t	*v_centermove;
cvar_t	*v_centerspeed;

cvar_t	*cl_bobcycle;
cvar_t	*cl_bob;
cvar_t	*cl_bobup;
cvar_t	*cl_waterdist;
cvar_t	*cl_chasedist;

vec3_t	g_CameraOrigin;
vec3_t	g_CameraAngles;

//
// view origin
//
vec3_t	vup;
vec3_t	vpn;
vec3_t	vright;
vec3_t	r_origin;

//----------------------------------------------------------------
// Purpose : Quakeworld bob code, this fixes jitters in the 
//           mutliplayer since the clock (pparams->time) isn't quite linear
//----------------------------------------------------------------
float V_CalcBob( ref_params_t *pparams )
{
	static double	bobtime;
	static float	bob;
	float			cycle;
	static float	lasttime;
	vec3_t			vel;

	if ( pparams->onground == -1 || pparams->time == lasttime )
	{
		// just use old value
		return bob;
	}

	lasttime = pparams->time;

	bobtime += pparams->frametime;
	cycle = bobtime - (int)( bobtime / cl_bobcycle->value ) * cl_bobcycle->value;
	cycle /= cl_bobcycle->value;

	if ( cycle < cl_bobup->value )
		cycle = M_PI * cycle / cl_bobup->value;
	else
		cycle = M_PI + M_PI * ( cycle - cl_bobup->value ) / ( 1.0 - cl_bobup->value );

	// bob is proportional to simulated velocity in the xy plane
	// (don't count Z, or jumping messes it up)
	VectorCopy( pparams->simvel, vel );
	vel[2] = 0;

	bob = sqrt( vel[0] * vel[0] + vel[1] * vel[1] ) * cl_bob->value;
	bob = bob * 0.3 + bob * 0.7 * sin( cycle );
	bob = min( bob, 4 );
	bob = max( bob, -7 );

	return bob;
}

//----------------------------------------------------------------
// Purpose : Used by view and sv_user
//----------------------------------------------------------------
float V_CalcRoll( vec3_t angles, vec3_t velocity, float rollangle, float rollspeed )
{
	float	sign;
	float	side;
	float	value;
	vec3_t	forward, right, up;

	gEngfuncs.pfnAngleVectors( angles, forward, right, up );

	side = DotProduct( velocity, right );
	sign = side < 0 ? -1 : 1;
	side = fabs( side );

	value = rollangle;

	if ( side < rollspeed )
		side = side * value / rollspeed;
	else
		side = value;

	return side * sign;
}

typedef struct pitchdrift_s
{
	float	pitchvel;
	int		nodrift;
	float	driftmove;
	double	laststop;

} pitchdrift_t;

static pitchdrift_t		pd;

//----------------------------------------------------------------
// Purpose : -
//----------------------------------------------------------------
void V_StartPitchDrift( void )
{
	if ( pd.laststop == gEngfuncs.GetClientTime() )
		return;

	if ( pd.nodrift || !pd.pitchvel )
	{
		pd.pitchvel = v_centerspeed->value;
		pd.nodrift = 0;
		pd.driftmove = 0;
	}
}

//----------------------------------------------------------------
// Purpose : -
//----------------------------------------------------------------
void V_StopPitchDrift( void )
{
	pd.laststop = gEngfuncs.GetClientTime();
	pd.nodrift = 1;
	pd.pitchvel = 0;
}

//----------------------------------------------------------------
// Purpose : Moves the client pitch angle towards idealpitch sent by the server.
//           If the user is adjusting pitch manually, either with lookup/lookdown,
//           mlook and mouse, or klook and keyboard, pitch drifting is constantly stopped.
//----------------------------------------------------------------
void V_DriftPitch( ref_params_t *pparams )
{
	float	delta, move;

	if ( gEngfuncs.IsNoClipping() || !pparams->onground || pparams->demoplayback || pparams->spectator )
	{
		pd.driftmove = 0;
		pd.pitchvel = 0;
		return;
	}

	// don't count small mouse motion
	if ( pd.nodrift )
	{
		if ( fabs( pparams->cmd->forwardmove ) < cl_forwardspeed->value )
			pd.driftmove = 0;
		else
			pd.driftmove += pparams->frametime;

		if ( pd.driftmove > v_centermove->value )
			V_StartPitchDrift();

		return;
	}

	delta = pparams->idealpitch - pparams->cl_viewangles[PITCH];

	if ( !delta )
	{
		pd.pitchvel = 0;
		return;
	}

	move = pparams->frametime * pd.pitchvel;
	pd.pitchvel += pparams->frametime * v_centerspeed->value;

	if ( delta > 0 )
	{
		if ( move > delta )
		{
			pd.pitchvel = 0;
			move = delta;
		}

		pparams->cl_viewangles[PITCH] += move;
	}
	else if ( delta < 0 )
	{
		if ( move > -delta )
		{
			pd.pitchvel = 0;
			move = -delta;
		}

		pparams->cl_viewangles[PITCH] -= move;
	}
}

//----------------------------------------------------------------
// Purpose : -
//----------------------------------------------------------------
void V_CalcViewRoll( ref_params_t *pparams )
{
	float		side;
	cl_entity_t	*viewentity;

	viewentity = gEngfuncs.GetEntityByIndex( pparams->viewentity );

	if ( !viewentity )
		return;

	side = V_CalcRoll( viewentity->angles, pparams->simvel, pparams->movevars->rollangle, pparams->movevars->rollspeed );

	pparams->viewangles[ROLL] += side;
}

//----------------------------------------------------------------
// Purpose : -
//----------------------------------------------------------------
void V_CalcIntermissionRefdef( ref_params_t *pparams )
{
	cl_entity_t	*view;

	view = gEngfuncs.GetViewModel();

	// Remove the gun model
	view->model = NULL;

	// Output
	VectorCopy( pparams->simorg, pparams->vieworg );
	VectorCopy( pparams->cl_viewangles, pparams->viewangles );

	// Save camera
	VectorCopy( pparams->vieworg, v_origin );
	VectorCopy( pparams->viewangles, v_angles );
	VectorCopy( pparams->cl_viewangles, v_cl_angles );
}

#define ORIGIN_BACKUP	64
#define ORIGIN_MASK		( ORIGIN_BACKUP - 1 )

typedef struct viewinterp_s
{
	float Origins[ ORIGIN_BACKUP ][3];
	float OriginTime[ ORIGIN_BACKUP ];

	float Angles[ ORIGIN_BACKUP ][3];
	float AngleTime[ ORIGIN_BACKUP ];

	int CurrentOrigin;
	int CurrentAngle;

} viewinterp_t;

//----------------------------------------------------------------
// Purpose : -
//----------------------------------------------------------------
void V_CalcNormalRefdef( ref_params_t *pparams )
{
	cl_entity_t				*ent, *view;
	int						i;
	vec3_t					angles;
	float					bob, waterOffset;

	static viewinterp_t		ViewInterp;
	static float			oldz = 0.0f;
	static float			lasttime;

	vec3_t					camAngles, camForward, camRight, camUp;
	cl_entity_t				*pwater;

	V_DriftPitch( pparams );

	if ( gEngfuncs.IsSpectateOnly() )
	{
		ent = gEngfuncs.GetEntityByIndex( ClientState.iUser2 );
	}
	else
	{
		// ent is the player model ( visible when out of body )
		ent = gEngfuncs.GetLocalPlayer();
	}

	// view is the weapon model (only visible from inside body )
	view = gEngfuncs.GetViewModel();

	// transform the view offset by the model's matrix to get the offset from
	// model origin for the view
	bob = V_CalcBob( pparams );

	// refresh position
	VectorCopy( pparams->simorg, pparams->vieworg );
	pparams->vieworg[2] += ( bob );
	VectorAdd( pparams->vieworg, pparams->viewheight, pparams->vieworg );

	VectorCopy( pparams->cl_viewangles, pparams->viewangles );

	gEngfuncs.V_CalcShake();
	gEngfuncs.V_ApplyShake( pparams->vieworg, pparams->viewangles, 1.0f );

	// never let view origin sit exactly on a node line, because a water plane can
	// dissapear when viewed with the eye exactly on it.
	// FIXME, we send origin at 1/128 now, change this?
	// the server protocol only specifies to 1/16 pixel, so add 1/32 in each axis

	pparams->vieworg[0] += 1.0f / 32.0f;
	pparams->vieworg[1] += 1.0f / 32.0f;
	pparams->vieworg[2] += 1.0f / 32.0f;

	// Check for problems around water, move the viewer artificially if necessary
	// this prevents drawing errors in GL due to waves

	waterOffset = 0.0f;

	if ( pparams->waterlevel >= 2 )
	{
		int		contents, waterDist, waterEntity;
		vec3_t	point;

		waterDist = cl_waterdist->value;

		if ( pparams->hardware )
		{
			waterEntity = gEngfuncs.PM_WaterEntity( pparams->simorg );

			if ( waterEntity >= 0 && waterEntity < pparams->max_entities )
			{
				pwater = gEngfuncs.GetEntityByIndex( waterEntity );

				if ( pwater && ( pwater->model != NULL ) )
				{
					// Add in wave height
					waterDist += ( pwater->curstate.scale * 16.0f );
				}
			}
		}
		else
		{
			// Don't need this in software
			waterEntity = 0;
		}

		VectorCopy( pparams->vieworg, point );

		// Eyes are above water, make sure we're above the waves
		if ( pparams->waterlevel == 2 )
		{
			point[2] -= waterDist;

			for ( i = 0; i < waterDist; i++ )
			{
				contents = gEngfuncs.PM_PointContents( point, NULL );

				if ( contents > CONTENTS_WATER )
					break;

				point[2] += 1.0f;
			}

			waterOffset = ( point[2] + waterDist ) - pparams->vieworg[2];
		}
		else
		{
			// eyes are under water. Make sure we're far enough under
			point[2] += waterDist;

			for ( i = 0; i < waterDist; i++ )
			{
				contents = gEngfuncs.PM_PointContents( point, NULL );

				if ( contents <= CONTENTS_WATER )
					break;

				point[2] -= 1.0f;
			}

			waterOffset = ( point[2] - waterDist ) - pparams->vieworg[2];
		}
	}

	pparams->vieworg[2] += waterOffset;

	V_CalcViewRoll( pparams );

	// offsets
	VectorCopy( pparams->cl_viewangles, angles );

	gEngfuncs.pfnAngleVectors( angles, pparams->forward, pparams->right, pparams->up );

	// Treating cam_ofs[2] as the distance
	if ( CL_IsThirdPerson() )
	{
	}

	// set up gun position
	VectorCopy( pparams->vieworg, view->origin );
	VectorCopy(pparams->vieworg, view->curstate.origin);

	// wtf ?
//	VectorSubtract( view->origin, pparams->up, view->origin );

	// Let the viewmodel shake at about 10% of the amplitude
	gEngfuncs.V_ApplyShake( view->origin, view->angles, 0.9f );

	// Add in the punchangle, if any
	VectorAdd( pparams->viewangles, pparams->punchangle, pparams->viewangles );

	// Give gun our viewangles
	VectorCopy(pparams->viewangles, view->angles);
	VectorCopy(pparams->viewangles, view->curstate.angles);

	// smooth out stair step ups
	if ( !pparams->smoothing && pparams->onground && pparams->simorg[2] - oldz > 0 )
	{
		float	steptime = pparams->time - lasttime;

		if ( steptime < 0 )
			steptime = 0;

		oldz += steptime * 150.0f;

		if ( oldz > pparams->simorg[2] )
			oldz = pparams->simorg[2];

		if ( pparams->simorg[2] - oldz > 18 )
			oldz = pparams->simorg[2] - 18;

		pparams->vieworg[2] += oldz - pparams->simorg[2];
		view->origin[2] += oldz - pparams->simorg[2];
	}
	else
	{
		oldz = pparams->simorg[2];
	}

	{
		static float lastorg[3];
		vec3_t delta;

		VectorSubtract( pparams->simorg, lastorg, delta );

		if ( Length( delta ) != 0.0 )
		{
			VectorCopy( pparams->simorg, ViewInterp.Origins[ViewInterp.CurrentOrigin & ORIGIN_MASK] );
			ViewInterp.OriginTime[ ViewInterp.CurrentOrigin & ORIGIN_MASK ] = pparams->time;
			ViewInterp.CurrentOrigin++;

			VectorCopy( pparams->simorg, lastorg );
		}
	}

	// Smooth out whole view in multiplayer when on trains, lifts
	if ( cl_vsmoothing && cl_vsmoothing->value && ( pparams->smoothing && ( pparams->maxclients > 1 ) ) )
	{
		int		foundidx;
		int		i;
		float	t;

		if ( cl_vsmoothing->value < 0.0f )
			cl_vsmoothing->value = 0.0f;

		t = pparams->time - cl_vsmoothing->value;

		for ( i = 1; i < ORIGIN_MASK; i++ )
		{
			foundidx = ViewInterp.CurrentOrigin - 1 - i;

			if ( ViewInterp.OriginTime[ foundidx & ORIGIN_MASK ] <= t )
				break;
		}

		if ( i < ORIGIN_MASK && ViewInterp.OriginTime[ foundidx & ORIGIN_MASK ] != 0.0f )
		{
			// Interpolate
			vec3_t	delta;
			double	frac;
			double	dt;
			vec3_t	neworg;

			dt = ViewInterp.OriginTime[ ( foundidx + 1 ) & ORIGIN_MASK ] - ViewInterp.OriginTime[ foundidx & ORIGIN_MASK ];

			if ( dt > 0.0f )
			{
				frac = ( t - ViewInterp.OriginTime[ foundidx & ORIGIN_MASK ] ) / dt;
				frac = min( 1.0f, frac );
				VectorSubtract( ViewInterp.Origins[ ( foundidx + 1 ) & ORIGIN_MASK ], ViewInterp.Origins[ foundidx & ORIGIN_MASK ], delta );
				VectorMA( ViewInterp.Origins[ foundidx & ORIGIN_MASK ], frac, delta, neworg );

				// Dont interpolate large changes
				if ( Length( delta ) < 64.0f )
				{
					VectorSubtract( neworg, pparams->simorg, delta );

					VectorAdd( pparams->simorg, delta, pparams->simorg );
					VectorAdd( pparams->vieworg, delta, pparams->vieworg );
					VectorAdd( view->origin, delta, view->origin );
				}
			}
		}
	}

	// Store off v_angles before munging for third person
	VectorCopy( pparams->viewangles, v_angles );
	VectorCopy( pparams->viewangles, v_lastAngles );

	if ( CL_IsThirdPerson() )
	{
	}

	// override all previous settings if the viewent isn't the client
	if (pparams->viewentity > pparams->maxclients)
	{
		cl_entity_t	*viewentity = gEngfuncs.GetEntityByIndex( pparams->viewentity );

		if ( viewentity )
		{
			VectorCopy( viewentity->origin, pparams->vieworg );
			VectorCopy( viewentity->angles, pparams->viewangles );

			// Store off overridden viewangles
			VectorCopy( pparams->viewangles, v_angles );
		}
	}

	lasttime = pparams->time;
	VectorCopy( pparams->vieworg, v_origin );
}

//----------------------------------------------------------------
// Purpose : -
//----------------------------------------------------------------
void V_SmoothInterpolateAngles( float *startAngle, float *endAngle, float *finalAngle, float degreesPerSec )
{
	float	absd, frac, d, threshhold;

	NormalizeAngles( startAngle );
	NormalizeAngles( endAngle );

	for ( int i = 0; i < 3; i++ )
	{
		d = endAngle[i] - startAngle[i];

		if ( d > 180.0f )
		{
			d -= 360.0f;
		}
		else if ( d < -180.0f )
		{
			d += 360.0f;
		}

		absd = fabs( d );

		if ( absd > 0.01f )
		{
			frac = degreesPerSec * v_frametime;

			threshhold = degreesPerSec / 4;
	
			if ( absd < threshhold )
			{
				float h = absd / threshhold;
				h *= h;
				frac*= h;	// slow down last degrees
			}

			if ( frac > absd )
			{
				finalAngle[i] = endAngle[i];
			}
			else
			{
				if ( d > 0 )
					finalAngle[i] = startAngle[i] + frac;
				else
					finalAngle[i] = startAngle[i] - frac;
			}
		}
		else
		{
			finalAngle[i] = endAngle[i];
		}
	}

	NormalizeAngles( finalAngle );
}

//----------------------------------------------------------------
// Purpose : Get the origin of the Observer based around 
//           the target's position and angles
//----------------------------------------------------------------
void V_GetChaseOrigin( float *angles, float *origin, float distance, float *returnvec )
{
	vec3_t		vecEnd;
	vec3_t		forward;
	vec3_t		vecStart;
	pmtrace_t	*trace;
	int			maxLoops = 8;

	// first, ignore no entity
	int ignoreent = -1;

	cl_entity_t *ent = NULL;

	// Trace back from the target using the player's view angles
	gEngfuncs.pfnAngleVectors( angles, forward, NULL, NULL );

	VectorScale( forward, -1, forward );

	VectorCopy( origin, vecStart );

	VectorMA( vecStart, distance, forward, vecEnd );

	while ( maxLoops > 0 )
	{
		trace = gEngfuncs.PM_TraceLine( vecStart, vecEnd, 0, 2, ignoreent );

		// WARNING! trace->ent is is the number in physent list not the normal entity number

		if ( trace->ent <= 0 )
			break;	// we hit the world or nothing, stop trace

		ent = gEngfuncs.GetEntityByIndex( PM_GetPhysEntInfo( trace->ent ) );

		if ( ent == NULL )
			break;

		// hit non-player solid BSP , stop here
		if ( ent->curstate.solid == SOLID_BSP && !ent->player )
			break;

		// if close enought to end pos, stop, otherwise continue trace
		if ( Distance( trace->endpos, vecEnd ) < 1.0f )
		{
			break;
		}
		else
		{
			ignoreent = trace->ent;	// ignore last hit entity
			VectorCopy(trace->endpos, vecStart);
		}

		maxLoops--;
	}

	VectorMA( trace->endpos, 4, trace->plane.normal, returnvec );

	// real distance without offset
	v_lastDistance = Distance( trace->endpos, origin );
}

//----------------------------------------------------------------
// Purpose : -
//----------------------------------------------------------------
void V_GetDeathCam( cl_entity_t *ent1, cl_entity_t *ent2, float *angle, float *origin )
{
	float	newAngle[3], newOrigin[3];

	float	distance = 168.0f;

	v_lastDistance += v_frametime * 96.0f;	// move unit per seconds back

//	if ( v_resetCamera )
//		v_lastDistance = 64.0f;

	if ( distance > v_lastDistance )
		distance = v_lastDistance;

	VectorCopy( ent1->origin, newOrigin );

	if ( ent1->player )
		newOrigin[2]+= VEC_IDLE_VIEW[2];	// head level of living player

	// get new angle towards second target
	if ( ent2 )
	{
		VectorSubtract( ent2->origin, ent1->origin, newAngle );
		VectorAngles( newAngle, newAngle );
		newAngle[0] = -newAngle[0];
	}
	else
	{
		// if no second target is given, look down to dead player
		newAngle[0] = 90.0f;
		newAngle[1] = 0.0f;
		newAngle[2] = 0;
	}

	// and smooth view
	V_SmoothInterpolateAngles( v_lastAngles, newAngle, angle, 120.0f );
			
	V_GetChaseOrigin( angle, newOrigin, distance, origin );

	VectorCopy( angle, v_lastAngles );
}

//----------------------------------------------------------------
// Purpose : -
//----------------------------------------------------------------
void V_GetSingleTargetCam( cl_entity_t *ent1, float *angle, float *origin )
{
	float	newAngle[3];
	float	newOrigin[3];

	//int flags = gHUD.m_Spectator.m_iObserverFlags;
	int flags = 0;

	// see is target is a dead player
	qboolean deadPlayer = ent1->player && ( ent1->curstate.solid == SOLID_NOT );

	float dfactor = ( flags & DRC_FLAG_DRAMATIC ) ? -1.0f : 1.0f;

	// get close if dramatic
	float distance = 112.0f + ( 16.0f * dfactor );

	// go away in final scenes or if player just died
	if ( flags & DRC_FLAG_FINAL )
		distance *= 2.0f;
	else if ( deadPlayer )
		distance *= 1.5f;

	// let v_lastDistance float smoothly away
	v_lastDistance += v_frametime * 32.0f;	// move unit per seconds back

	if ( distance > v_lastDistance )
		distance = v_lastDistance;

	VectorCopy( ent1->origin, newOrigin );

	if ( ent1->player )
	{
		if ( deadPlayer )
			newOrigin[2] += 2;	// laying on ground
		else
			newOrigin[2] += 17;	// head level of living player
	}
	else
		newOrigin[2] += 8;	// object, tricky, must be above bomb in CS

	// we have no second target, choose view direction based on
	// show front of primary target
	VectorCopy( ent1->angles, newAngle );

	// show dead players from front, normal players back
	if ( flags & DRC_FLAG_FACEPLAYER )
		newAngle[1] += 180.0f;

	// lower angle if dramatic
	newAngle[0] += 12.5f * dfactor;

	// if final scene (bomb), show from real high pos
	if ( flags & DRC_FLAG_FINAL )
		newAngle[0] = 22.5f;

	// choose side of object/player
	if ( flags & DRC_FLAG_SIDE )
		newAngle[1] += 22.5f;
	else
		newAngle[1] -= 22.5f;

	V_SmoothInterpolateAngles( v_lastAngles, newAngle, angle, 120.0f );

	// HACK, if player is dead don't clip against his dead body, can't check this
	V_GetChaseOrigin( angle, newOrigin, distance, origin );
}

//----------------------------------------------------------------
// Purpose : -
//----------------------------------------------------------------
float MaxAngleBetweenAngles( float *a1, float *a2 )
{
	int		i;
	float	d, maxd = 0.0f;

	NormalizeAngles( a1 );
	NormalizeAngles( a2 );

	for ( i = 0; i < 3; i++ )
	{
		d = a2[i] - a1[i];

		if ( d > 180.0f )
		{
			d -= 360.0f;
		}
		else if ( d < -180.0f )
		{
			d += 360.0f;
		}

		d = fabs( d );

		if ( d > maxd )
			maxd = d;
	}

	return maxd;
}

//----------------------------------------------------------------
// Purpose : -
//----------------------------------------------------------------
void V_GetChasePos( int target, float *cl_angles, float *origin, float *angles )
{
	cl_entity_t	*ent = NULL;

	if ( target )
		ent = gEngfuncs.GetEntityByIndex( target );

	if ( !ent )
	{
		// just copy a save in-map position
		VectorCopy( vJumpAngles, angles );
		VectorCopy( vJumpOrigin, origin );
		return;
	}

	if ( ent->index == gEngfuncs.GetLocalPlayer()->index )
	{
		if (ClientState.iUser3)
			V_GetDeathCam( ent, gEngfuncs.GetEntityByIndex( ClientState.iUser3 ), angles, origin );
		else
			V_GetDeathCam( ent, NULL, angles, origin );
	}
	else
	{
		// no mouse angles given, use entity angles ( locked mode )
		if ( cl_angles == NULL )
		{
			VectorCopy( ent->angles, angles );
			angles[0] *= -1;
		}
		else
			VectorCopy( cl_angles, angles );

		VectorCopy( ent->origin, origin );

		//  DEFAULT_VIEWHEIGHT - some offset
		origin[2] += VEC_IDLE_VIEW[2];

		V_GetChaseOrigin( angles, origin, cl_chasedist->value, origin );
	}
}

//----------------------------------------------------------------
// Purpose : -
//----------------------------------------------------------------
void V_GetInEyePos( int target, float *origin, float *angles )
{
	if ( !target )
	{
		VectorCopy( vJumpAngles, angles );
		VectorCopy( vJumpOrigin, origin );
		return;
	}

	cl_entity_t	*ent = gEngfuncs.GetEntityByIndex( target );

	if ( !ent )
		return;

	VectorCopy( ent->origin, origin );
	VectorCopy( ent->angles, angles );

	// see CL_ProcessEntityUpdate()
	angles[PITCH] *= -3.0f;

	if ( ent->curstate.solid == SOLID_NOT )
	{
		// dead view
	}
	else if ( ent->curstate.usehull == 1 )
	{
		origin[2] += VEC_DUCK_VIEW[2];
	}
	else
	{
		// exacty eye position can't be caluculated since it depends on
		//  client values like cl_bobcycle, this offset matches the default values
		origin[2] += VEC_IDLE_VIEW[2];
	}
}

//----------------------------------------------------------------
// Purpose : -
//----------------------------------------------------------------
void V_CalcSpectatorRefdef( ref_params_t *pparams )
{
	static vec3_t	velocity = { 0.0f };

	static int lastWeaponModelIndex = 0;
	static int lastViewModelIndex = 0;

	cl_entity_t	*ent = gEngfuncs.GetEntityByIndex( ClientState.iUser2 );

	pparams->onlyClientDraw = false;

	// refresh position
	VectorCopy( pparams->simorg, v_sim_org );

	// get old values
	VectorCopy( pparams->cl_viewangles, v_cl_angles );
	VectorCopy( pparams->viewangles, v_angles );
	VectorCopy( pparams->vieworg, v_origin );

	if ( ClientState.iUser1 == OBS_IN_EYE && ent )
	{
		// calculate player velocity
		float timeDiff = ent->curstate.msg_time - ent->prevstate.msg_time;

		if ( timeDiff > 0 )
		{
			vec3_t	distance;
			VectorSubtract( ent->prevstate.origin, ent->curstate.origin, distance );
			VectorScale( distance, 1 / timeDiff, distance );

			velocity[0] = velocity[0] * 0.9f + distance[0] * 0.1f;
			velocity[1] = velocity[1] * 0.9f + distance[1] * 0.1f;
			velocity[2] = velocity[2] * 0.9f + distance[2] * 0.1f;

			VectorCopy( velocity, pparams->simvel );
		}

		// predict missing client data and set weapon model ( in HLTV mode or inset in eye mode )
		if ( gEngfuncs.IsSpectateOnly() )
		{
			V_GetInEyePos( ClientState.iUser2, pparams->simorg, pparams->cl_viewangles );

			pparams->health = 1;
		}
		else
		{
			// only get viewangles from entity
			VectorCopy( ent->angles, pparams->cl_viewangles );

			// see CL_ProcessEntityUpdate()
			pparams->cl_viewangles[PITCH] *= -3.0f;
		}
	}

	v_frametime = pparams->frametime;

	if ( pparams->nextView == 0 )
	{
		// first renderer cycle, full screen

		switch ( ClientState.iUser1 )
		{
			case OBS_CHASE_LOCKED:
			{
				V_GetChasePos( ClientState.iUser2, NULL, v_origin, v_angles );
				break;
			}

			case OBS_CHASE_FREE:
			{
				V_GetChasePos( ClientState.iUser2, v_cl_angles, v_origin, v_angles );
				break;
			}

			case OBS_ROAMING:
			{
				VectorCopy( v_cl_angles, v_angles );
				VectorCopy( v_sim_org, v_origin );
				break;
			}

			case OBS_IN_EYE:
			{
				V_CalcNormalRefdef(pparams);
				break;
			}

			case OBS_MAP_FREE:
			case OBS_MAP_CHASE:
			{
				break;
			}
		}
	}
	else
	{
		// second renderer cycle, inset window
		// picture in picture
	}

	// write back new values into pparams
	VectorCopy( v_cl_angles, pparams->cl_viewangles );
	VectorCopy( v_angles, pparams->viewangles );
	VectorCopy( v_origin, pparams->vieworg );
}

//----------------------------------------------------------------
// Purpose : -
//----------------------------------------------------------------
DLLExport void V_CalcRefdef( ref_params_t *pparams )
{
	if ( pparams->intermission )
	{
		V_CalcIntermissionRefdef( pparams );
	}
	else if ( pparams->spectator || ClientState.iUser1 )
	{
		V_CalcSpectatorRefdef(pparams);
	}
	else if ( !pparams->paused )
	{
		V_CalcNormalRefdef( pparams );
	}

	VectorCopy( pparams->vieworg, g_CameraOrigin );
	VectorCopy( pparams->viewangles, g_CameraAngles );

	VectorCopy( pparams->up, vup );
	VectorCopy( pparams->forward, vpn );
	VectorCopy( pparams->right, vright );
	VectorCopy( pparams->vieworg, v_origin );
}

//----------------------------------------------------------------
// Purpose : -
//----------------------------------------------------------------
void V_DropPunchAngle( float frametime, float *ev_punchangle )
{
	float	len;

	len = VectorNormalize( ev_punchangle );
	len -= ( 10.0f + len * 0.5f ) * frametime;
	len = max( len, 0.0f );
	VectorScale( ev_punchangle, len, ev_punchangle );
}

//----------------------------------------------------------------
// Purpose : -
//----------------------------------------------------------------
void V_Init( void )
{
	gEngfuncs.pfnAddCommand( "centerview", V_StartPitchDrift );

	cl_vsmoothing = gEngfuncs.pfnRegisterVariable( "cl_vsmoothing", "0.05", FCVAR_CLIENTDLL );

	v_centermove = gEngfuncs.pfnRegisterVariable( "v_centermove", "0.15", FCVAR_CLIENTDLL );
	v_centerspeed = gEngfuncs.pfnRegisterVariable( "v_centerspeed", "500.0", FCVAR_CLIENTDLL );

	cl_bobcycle = gEngfuncs.pfnRegisterVariable( "cl_bobcycle", "0.8", FCVAR_CLIENTDLL );
	cl_bob = gEngfuncs.pfnRegisterVariable( "cl_bob", "0.01", FCVAR_CLIENTDLL );
	cl_bobup = gEngfuncs.pfnRegisterVariable( "cl_bobup", "0.5", FCVAR_CLIENTDLL );
	cl_waterdist = gEngfuncs.pfnRegisterVariable( "cl_waterdist", "4.0", FCVAR_CLIENTDLL );
	cl_chasedist = gEngfuncs.pfnRegisterVariable( "cl_chasedist", "112.0", FCVAR_CLIENTDLL );
}