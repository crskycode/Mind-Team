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

#ifndef EV_HLDM_H
#define EV_HLDM_H

void EV_KnifeAttack(struct event_args_s *args);
void EV_RifleFire(struct event_args_s *args);
void EV_PistolFire(struct event_args_s *args);
void EV_SniperFire(struct event_args_s *args);
void EV_ShotgunFire(struct event_args_s *args);
void EV_Grenade(struct event_args_s *args);

#endif