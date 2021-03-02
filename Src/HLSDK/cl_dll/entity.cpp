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

#include <studio_event.h>
#include <r_efx.h>
#include <event_api.h>
#include <pm_defs.h>
#include <pm_shared.h>

#include <ITextureAPI.h>
#include "MuzzleFlash.h"

extern "C" 
{
	DLLExport BOOL HUD_AddEntity( int type, struct cl_entity_s *ent, const char *modelname );
	DLLExport void HUD_CreateEntities( void );
	DLLExport void HUD_StudioEvent( const struct mstudioevent_s *event, const struct cl_entity_s *entity );
	DLLExport void HUD_TxferLocalOverrides( struct entity_state_s *state, const struct clientdata_s *client );
	DLLExport void HUD_ProcessPlayerState( struct entity_state_s *dst, const struct entity_state_s *src );
	DLLExport void HUD_TxferPredictionData( struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd );
	DLLExport void HUD_TempEntUpdate( double frametime, double client_time, double cl_gravity, struct tempent_s **ppTempEntFree, struct tempent_s **ppTempEntActive, int (*Callback_AddVisibleEntity)(struct cl_entity_s *pEntity), void (*Callback_TempEntPlaySound)(struct tempent_s *pTemp, float damp) );
	DLLExport cl_entity_t *HUD_GetUserEntity( int index );
}

//----------------------------------------------------------------
// Purpose : Return 0 to filter entity from visible list for rendering
//----------------------------------------------------------------
DLLExport BOOL HUD_AddEntity( int type, struct cl_entity_s *ent, const char *modelname )
{
	// each frame every entity passes this function, so the overview hooks it to filter the overview entities
	// in spectator mode:
	// each frame every entity passes this function, so the overview hooks
	// it to filter the overview entities
	if ( ClientState.iUser1 )
	{
		if ( ClientState.iUser1 == OBS_IN_EYE && ent->index == ClientState.iUser2 )
		{
			// don't draw the player we are following in eye
			return FALSE;
		}
	}

	return TRUE;
}

//----------------------------------------------------------------
// Purpose : The server sends us our origin with extra precision as part of the clientdata structure, not during the normal
//           playerstate update in entity_state_t.  In order for these overrides to eventually get to the appropriate playerstate
//           structure, we need to copy them into the state structure at this point.
//----------------------------------------------------------------
DLLExport void HUD_TxferLocalOverrides( struct entity_state_s *state, const struct clientdata_s *client )
{
	VectorCopy( client->origin, state->origin );

	state->iuser1 = client->iuser1;
	state->iuser2 = client->iuser2;
	state->iuser3 = client->iuser3;
}

//----------------------------------------------------------------
// Purpose : We have received entity_state_t for this player over the network.  We need to copy appropriate fields to the
//           playerstate structure
//----------------------------------------------------------------
DLLExport void HUD_ProcessPlayerState( struct entity_state_s *dst, const struct entity_state_s *src )
{
	// Copy in network data
	VectorCopy( src->origin, dst->origin );
	VectorCopy( src->angles, dst->angles );

	VectorCopy( src->velocity, dst->velocity );

	dst->frame = src->frame;
	dst->modelindex = src->modelindex;
	dst->skin = src->skin;
	dst->effects = src->effects;
	dst->weaponmodel = src->weaponmodel;
	dst->movetype = src->movetype;
	dst->sequence = src->sequence;
	dst->animtime = src->animtime;
	
	dst->solid = src->solid;
	
	dst->rendermode = src->rendermode;
	dst->renderamt = src->renderamt;	
	dst->rendercolor.r = src->rendercolor.r;
	dst->rendercolor.g = src->rendercolor.g;
	dst->rendercolor.b = src->rendercolor.b;
	dst->renderfx = src->renderfx;

	dst->framerate = src->framerate;
	dst->body = src->body;

	VectorCopy( src->basevelocity, dst->basevelocity );

	dst->friction = src->friction;
	dst->gravity = src->gravity;
	dst->gaitsequence = src->gaitsequence;
	dst->spectator = src->spectator;
	dst->usehull = src->usehull;
	dst->playerclass = src->playerclass;
	dst->team = src->team;
	dst->colormap = src->colormap;

	// Save off some data so other areas of the Client DLL can get to it

	// Get the local player's index
	cl_entity_t *player = gEngfuncs.GetLocalPlayer();

	if ( dst->number == player->index )
	{
		ClientState.iUser1 = src->iuser1;
		ClientState.iUser2 = src->iuser2;
		ClientState.iUser3 = src->iuser3;
	}
}

//----------------------------------------------------------------
// Purpose : Because we can predict an arbitrary number of frames before the server responds with an update, we need to be able to copy client side prediction data in
//           from the state that the server ack'd receiving, which can be anywhere along the predicted frame path ( i.e., we could predict 20 frames into the future and the server ack's
//           up through 10 of those frames, so we need to copy persistent client-side only state from the 10th predicted frame to the slot the server
//           update is occupying.
//----------------------------------------------------------------
DLLExport void HUD_TxferPredictionData( struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd )
{
	ps->oldbuttons			= pps->oldbuttons;
	ps->flFallVelocity		= pps->flFallVelocity;
	ps->iStepLeft			= pps->iStepLeft;
	ps->playerclass			= pps->playerclass;

	pcd->viewmodel			= ppcd->viewmodel;
	pcd->m_iId				= ppcd->m_iId;
	pcd->ammo_shells		= ppcd->ammo_shells;
	pcd->ammo_nails			= ppcd->ammo_nails;
	pcd->ammo_cells			= ppcd->ammo_cells;
	pcd->ammo_rockets		= ppcd->ammo_rockets;
	pcd->m_flNextAttack		= ppcd->m_flNextAttack;
	pcd->fov				= ppcd->fov;
	pcd->weaponanim			= ppcd->weaponanim;
	pcd->tfstate			= ppcd->tfstate;
	pcd->maxspeed			= ppcd->maxspeed;

	pcd->deadflag			= ppcd->deadflag;

	ClientState.bIsAlive = (/*ppcd->iuser1 || */pcd->deadflag == DEAD_NO) ? TRUE : FALSE;

	ClientState.bDucking	= (ppcd->flags & FL_DUCKING) ? TRUE : FALSE;
	ClientState.iFlags		= ppcd->flags;

	VectorCopy(ppcd->velocity, ClientState.vecVelocity);

	pcd->iuser1				= ppcd->iuser1;
	pcd->iuser2				= ppcd->iuser2;
	pcd->iuser3				= ppcd->iuser3;

	// 作为观察者的时候从服务器复制...
	if (ClientState.iUser1 != OBS_NONE)
	{
		VectorCopy(pps->origin, ClientState.vecOrigin);
		VectorCopy(pps->angles, ClientState.vecAngles);
	}

	// in specator mode we tell the engine who we want to spectate and how
	// iuser3 is not used for duck prevention (since the spectator can't duck at all)
	if ( gEngfuncs.IsSpectateOnly() )
	{
		pcd->iuser1 = ClientState.iUser1;
		pcd->iuser2 = ClientState.iUser2;
		pcd->iuser3 = ClientState.iUser3;
	}

	pcd->fuser2				= ppcd->fuser2;
	pcd->fuser3				= ppcd->fuser3;

	VectorCopy(ppcd->vuser1, pcd->vuser1);
	VectorCopy(ppcd->vuser2, pcd->vuser2);
	VectorCopy(ppcd->vuser3, pcd->vuser3);
	VectorCopy(ppcd->vuser4, pcd->vuser4);

	ViewModel.iTeam = ppcd->vuser1[0];
	ViewModel.iCharacterId = ppcd->vuser1[1];
	ViewModel.iWeaponId = ppcd->vuser1[2];
}

//----------------------------------------------------------------
// Purpose : Gives us a chance to add additional entities to the render this frame
//----------------------------------------------------------------
DLLExport void HUD_CreateEntities(void)
{
}

//----------------------------------------------------------------
// Purpose : The entity's studio model description indicated an event was
//           fired during this frame, handle the event by it's tag ( e.g., muzzleflash, sound )
//----------------------------------------------------------------
DLLExport void HUD_StudioEvent(const struct mstudioevent_s *event, const struct cl_entity_s *entity)
{
	switch(event->event)
	{
	case 5001:
		break;
	case 5011:
		gEngfuncs.pEfxAPI->R_MuzzleFlash((float *)&entity->attachment[1], atoi( event->options));
		break;
	case 5021:
		gEngfuncs.pEfxAPI->R_MuzzleFlash((float *)&entity->attachment[2], atoi( event->options));
		break;
	case 5031:
		gEngfuncs.pEfxAPI->R_MuzzleFlash((float *)&entity->attachment[3], atoi( event->options));
		break;
	case 5002:
		break;
	case 5004:
		gEngfuncs.pfnPlaySoundByNameAtLocation((char *)event->options, 1.0, (float *)&entity->attachment[0]);
		break;
	}
}

//----------------------------------------------------------------
// Purpose : Simulation and cleanup of temporary entities
//----------------------------------------------------------------
DLLExport void HUD_TempEntUpdate(
	double			frametime,			// Simulation time
	double			client_time,		// Absolute time on client
	double			cl_gravity,			// True gravity on client
	TEMPENTITY		**ppTempEntFree,	// List of freed temporary ents
	TEMPENTITY		**ppTempEntActive,	// List
	int				(*Callback_AddVisibleEntity)(cl_entity_t *pEntity),
	void			(*Callback_TempEntPlaySound)(TEMPENTITY *pTemp, float damp))
{
	static int		gTempEntFrame = 0;
	int				i;
	TEMPENTITY		*pTemp, *pnext, *pprev;
	float			freq, gravity, gravitySlow, life, fastFreq;

	// Nothing to simulate
	if ( !*ppTempEntActive )
		return;

	// in order to have tents collide with players, we have to run the player prediction code so
	// that the client has the player list. We run this code once when we detect any COLLIDEALL
	// tent, then set this BOOL to true so the code doesn't get run again if there's more than
	// one COLLIDEALL ent for this update. (often are).
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers( -1 );

	// !!!BUG -- This needs to be time based
	gTempEntFrame = ( gTempEntFrame + 1 ) & 31;

	pTemp = *ppTempEntActive;

	// !!! Don't simulate while paused....  This is sort of a hack, revisit.
	if ( frametime <= 0 )
	{
		while ( pTemp )
		{
			if ( !( pTemp->flags & FTENT_NOMODEL ) )
			{
				Callback_AddVisibleEntity( &pTemp->entity );
			}

			pTemp = pTemp->next;
		}

		goto finish;
	}

	pprev = NULL;
	freq = client_time * 0.01;
	fastFreq = client_time * 5.5;
	gravity = -frametime * cl_gravity;
	gravitySlow = gravity * 0.5;

	while ( pTemp )
	{
		int active = 1;

		life = pTemp->die - client_time;
		pnext = pTemp->next;

		if ( life < 0 )
		{
			if ( pTemp->flags & FTENT_FADEOUT )
			{
				if ( pTemp->entity.curstate.rendermode == kRenderNormal )
				{
					pTemp->entity.curstate.rendermode = kRenderTransTexture;
				}

				pTemp->entity.curstate.renderamt = pTemp->entity.baseline.renderamt * (1 + life * pTemp->fadeSpeed);

				if ( pTemp->entity.curstate.renderamt <= 0 )
				{
					active = 0;
				}
			}
			else
			{
				active = 0;
			}
		}

		if ( !active )	// Kill it
		{
			pTemp->next = *ppTempEntFree;
			*ppTempEntFree = pTemp;

			if ( !pprev )	// Deleting at head of list
			{
				*ppTempEntActive = pnext;
			}
			else
				pprev->next = pnext;
		}
		else
		{
			pprev = pTemp;
			
			VectorCopy( pTemp->entity.origin, pTemp->entity.prevstate.origin );

			if ( pTemp->flags & FTENT_SPARKSHOWER )
			{
				// Adjust speed if it's time
				// Scale is next think time
				if ( client_time > pTemp->entity.baseline.scale )
				{
					// Show Sparks
					gEngfuncs.pEfxAPI->R_SparkEffect( pTemp->entity.origin, 8, -200, 200 );

					// Reduce life
					pTemp->entity.baseline.framerate -= 0.1;

					if ( pTemp->entity.baseline.framerate <= 0.0 )
					{
						pTemp->die = client_time;
					}
					else
					{
						// So it will die no matter what
						pTemp->die = client_time + 0.5;

						// Next think
						pTemp->entity.baseline.scale = client_time + 0.1;
					}
				}
			}
			else if ( pTemp->flags & FTENT_PLYRATTACHMENT )
			{
				cl_entity_t *pClient = gEngfuncs.GetEntityByIndex( pTemp->clientIndex );

				VectorAdd( pClient->origin, pTemp->tentOffset, pTemp->entity.origin );
			}
			else if ( pTemp->flags & FTENT_SINEWAVE )
			{
				pTemp->x += pTemp->entity.baseline.origin[0] * frametime;
				pTemp->y += pTemp->entity.baseline.origin[1] * frametime;

				pTemp->entity.origin[0] = pTemp->x + sin( pTemp->entity.baseline.origin[2] + client_time * pTemp->entity.prevstate.frame ) * ( 10 * pTemp->entity.curstate.framerate );
				pTemp->entity.origin[1] = pTemp->y + sin( pTemp->entity.baseline.origin[2] + fastFreq + 0.7 ) * ( 8 * pTemp->entity.curstate.framerate );
				pTemp->entity.origin[2] += pTemp->entity.baseline.origin[2] * frametime;
			}
			else if ( pTemp->flags & FTENT_SPIRAL )
			{
				float	s, c;
				s = sin( pTemp->entity.baseline.origin[2] + fastFreq );
				c = cos( pTemp->entity.baseline.origin[2] + fastFreq );

				pTemp->entity.origin[0] += pTemp->entity.baseline.origin[0] * frametime + 8 * sin( client_time * 20 + (int)pTemp );
				pTemp->entity.origin[1] += pTemp->entity.baseline.origin[1] * frametime + 4 * sin( client_time * 30 + (int)pTemp );
				pTemp->entity.origin[2] += pTemp->entity.baseline.origin[2] * frametime;
			}
			else
			{
				for ( i = 0; i < 3; i++ )
				{
					pTemp->entity.origin[i] += pTemp->entity.baseline.origin[i] * frametime;
				}
			}
			
			if ( pTemp->flags & FTENT_SPRANIMATE )
			{
				pTemp->entity.curstate.frame += frametime * pTemp->entity.curstate.framerate;

				if ( pTemp->entity.curstate.frame >= pTemp->frameMax )
				{
					pTemp->entity.curstate.frame = pTemp->entity.curstate.frame - (int)(pTemp->entity.curstate.frame);

					if ( !( pTemp->flags & FTENT_SPRANIMATELOOP ) )
					{
						// this animating sprite isn't set to loop, so destroy it.
						pTemp->die = client_time;
						pTemp = pnext;
						continue;
					}
				}
			}
			else if ( pTemp->flags & FTENT_SPRCYCLE )
			{
				pTemp->entity.curstate.frame += frametime * 10;

				if ( pTemp->entity.curstate.frame >= pTemp->frameMax )
				{
					pTemp->entity.curstate.frame = pTemp->entity.curstate.frame - (int)( pTemp->entity.curstate.frame );
				}
			}

			if ( pTemp->flags & FTENT_ROTATE )
			{
				pTemp->entity.angles[0] += pTemp->entity.baseline.angles[0] * frametime;
				pTemp->entity.angles[1] += pTemp->entity.baseline.angles[1] * frametime;
				pTemp->entity.angles[2] += pTemp->entity.baseline.angles[2] * frametime;

				VectorCopy( pTemp->entity.angles, pTemp->entity.latched.prevangles );
			}

			if ( pTemp->flags & ( FTENT_COLLIDEALL | FTENT_COLLIDEWORLD ) )
			{
				vec3_t	traceNormal;
				float	traceFraction = 1;

				if ( pTemp->flags & FTENT_COLLIDEALL )
				{
					pmtrace_t	pmtrace;
					physent_t	*pe;
				
					gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );

					gEngfuncs.pEventAPI->EV_PlayerTrace( pTemp->entity.prevstate.origin, pTemp->entity.origin, PM_STUDIO_BOX, -1, &pmtrace );


					if ( pmtrace.fraction != 1 )
					{
						pe = gEngfuncs.pEventAPI->EV_GetPhysent( pmtrace.ent );

						if ( !pmtrace.ent || ( pe->info != pTemp->clientIndex ) )
						{
							traceFraction = pmtrace.fraction;
							VectorCopy( pmtrace.plane.normal, traceNormal );

							if ( pTemp->hitcallback )
							{
								pTemp->hitcallback( pTemp, &pmtrace );
							}
						}
					}
				}
				else if ( pTemp->flags & FTENT_COLLIDEWORLD )
				{
					pmtrace_t	pmtrace;
					
					gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );

					gEngfuncs.pEventAPI->EV_PlayerTrace( pTemp->entity.prevstate.origin, pTemp->entity.origin, PM_STUDIO_BOX | PM_WORLD_ONLY, -1, &pmtrace );

					if ( pmtrace.fraction != 1 )
					{
						traceFraction = pmtrace.fraction;
						VectorCopy( pmtrace.plane.normal, traceNormal );

						if ( pTemp->flags & FTENT_SPARKSHOWER )
						{
							// Chop spark speeds a bit more
							VectorScale( pTemp->entity.baseline.origin, 0.6, pTemp->entity.baseline.origin );

							if ( Length( pTemp->entity.baseline.origin ) < 10 )
							{
								pTemp->entity.baseline.framerate = 0;
							}
						}

						if ( pTemp->hitcallback )
						{
							pTemp->hitcallback( pTemp, &pmtrace );
						}
					}
				}

				// Decent collision now, and damping works
				if ( traceFraction != 1 )
				{
					float	proj, damp;

					// Place at contact point
					VectorMA( pTemp->entity.prevstate.origin, traceFraction*frametime, pTemp->entity.baseline.origin, pTemp->entity.origin );
					// Damp velocity
					damp = pTemp->bounceFactor;

					if ( pTemp->flags & ( FTENT_GRAVITY|FTENT_SLOWGRAVITY ) )
					{
						damp *= 0.5f;

						// Hit floor?
						if ( traceNormal[2] > 0.9f )
						{
							if ( pTemp->entity.baseline.origin[2] <= 0.0f && pTemp->entity.baseline.origin[2] >= gravity * 3.0f )
							{
								damp = 0.0f;	// Stop
								pTemp->flags &= ~( FTENT_ROTATE | FTENT_GRAVITY | FTENT_SLOWGRAVITY | FTENT_COLLIDEWORLD | FTENT_SMOKETRAIL );
								pTemp->entity.angles[0] = 0.0f;
								pTemp->entity.angles[2] = 0.0f;
							}
						}
					}

					if ( pTemp->hitSound )
					{
						Callback_TempEntPlaySound( pTemp, damp );
					}

					if ( pTemp->flags & FTENT_COLLIDEKILL )
					{
						// die on impact
						pTemp->flags &= ~FTENT_FADEOUT;
						pTemp->die = client_time;
					}
					else
					{
						// Reflect velocity
						if ( damp != 0 )
						{
							proj = DotProduct( pTemp->entity.baseline.origin, traceNormal );
							VectorMA( pTemp->entity.baseline.origin, -proj*2, traceNormal, pTemp->entity.baseline.origin );
							// Reflect rotation (fake)

							pTemp->entity.angles[1] = -pTemp->entity.angles[1];
						}
						
						if ( damp != 1 )
						{
							VectorScale( pTemp->entity.baseline.origin, damp, pTemp->entity.baseline.origin );
							VectorScale( pTemp->entity.angles, 0.9, pTemp->entity.angles );
						}
					}
				}
			}

			if ( ( pTemp->flags & FTENT_FLICKER ) && gTempEntFrame == pTemp->entity.curstate.effects )
			{
				dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight( 0 );
				VectorCopy( pTemp->entity.origin, dl->origin );
				dl->radius = 60;
				dl->color.r = 255;
				dl->color.g = 120;
				dl->color.b = 0;
				dl->die = client_time + 0.01;
			}

			if ( pTemp->flags & FTENT_SMOKETRAIL )
			{
				gEngfuncs.pEfxAPI->R_RocketTrail( pTemp->entity.prevstate.origin, pTemp->entity.origin, 1 );
			}

			if ( pTemp->flags & FTENT_GRAVITY )
				pTemp->entity.baseline.origin[2] += gravity;
			else if ( pTemp->flags & FTENT_SLOWGRAVITY )
				pTemp->entity.baseline.origin[2] += gravitySlow;

			if ( pTemp->flags & FTENT_CLIENTCUSTOM )
			{
				if ( pTemp->callback )
				{
					pTemp->callback( pTemp, frametime, client_time );
				}
			}

			// Cull to PVS (not frustum cull, just PVS)
			if ( !( pTemp->flags & FTENT_NOMODEL ) )
			{
				if ( !Callback_AddVisibleEntity( &pTemp->entity ) )
				{
					if( !( pTemp->flags & FTENT_PERSIST ) )
					{
						pTemp->die = client_time;	// If we can't draw it this frame, just dump it.
						pTemp->flags &= ~FTENT_FADEOUT;	// Don't fade out, just die
					}
				}
			}
		}

		pTemp = pnext;
	}

finish:
	// Restore state info
	gEngfuncs.pEventAPI->EV_PopPMStates();
}

//----------------------------------------------------------------
// Purpose : If you specify negative numbers for beam start and end point entities, then
//           the engine will call back into this function requesting a pointer to a cl_entity_t
//           object that describes the entity to attach the beam onto.
//
//           Indices must start at 1, not zero.
//----------------------------------------------------------------
DLLExport cl_entity_t *HUD_GetUserEntity( int index )
{
	// None by default, you would return a valic pointer if you create a client side
	// beam and attach it to a client side entity.
	return NULL;
}