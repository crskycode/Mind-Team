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
#include "ev_hldm.h"

//----------------------------------------------------------------
// Purpose : 
//----------------------------------------------------------------
void EV_HookEvents(void)
{
	gEngfuncs.pfnHookEvent("events/knifeattack.sc", EV_KnifeAttack);
	gEngfuncs.pfnHookEvent("events/riflefire.sc", EV_RifleFire);
	gEngfuncs.pfnHookEvent("events/pistolfire.sc", EV_PistolFire);
	gEngfuncs.pfnHookEvent("events/sniperfire.sc", EV_SniperFire);
	gEngfuncs.pfnHookEvent("events/shotgunfire.sc", EV_ShotgunFire);
	gEngfuncs.pfnHookEvent("events/grenade.sc", EV_Grenade);
}