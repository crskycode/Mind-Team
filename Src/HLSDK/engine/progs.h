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

#ifndef PROGS_H
#define PROGS_H

#include "progdefs.h"

// 16 simultaneous events, max
#define MAX_EVENT_QUEUE 64

#define DEFAULT_EVENT_RESENDS 1

#include "event_flags.h"
#include "event_args.h"

typedef struct event_info_s
{
	unsigned short	index;				// 0 implies not in use
	short			packet_index;		// Use data from state info for entity in delta_packet .  -1 implies separate info based on event
	short			entity_index;		// The edict this event is associated with
	float			fire_time;			// if non-zero, the time when the event should be fired ( fixed up on the client )
	event_args_t	args;
	// CLIENT ONLY	
	int				flags;				// Reliable or not, etc.

} event_info_t;

typedef struct event_state_s
{
	struct event_info_s		ei[MAX_EVENT_QUEUE];

} event_state_t;

#include "entity_state.h"
#include "edict.h"

#define STRUCT_FROM_LINK(l,t,m) ((t *)((byte *)l - (int)&(((t *)0)->m)))
#define EDICT_FROM_AREA(l) STRUCT_FROM_LINK(l,edict_t,area)

#endif