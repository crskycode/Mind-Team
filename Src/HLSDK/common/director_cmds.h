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

#ifndef DIRECTOR_CMDS_H
#define DIRECTOR_CMDS_H

#define DRC_ACTIVE					0		// tells client that he's an spectator and will get director command
#define DRC_STATUS					1		// send status infos about proxy 
#define DRC_CAMERA					2		// set the actual director camera position
#define DRC_EVENT					3		// informs the dircetor about ann important game event

#define DRC_FLAG_PRIO_MASK			0x0F	//	priorities between 0 and 15 (15 most important)
#define DRC_FLAG_SIDE				(1<<4)	
#define DRC_FLAG_DRAMATIC			(1<<5)

// commands of the director API function CallDirectorProc(...)

#define DRCAPI_NOP					0		// no operation
#define DRCAPI_ACTIVE				1		// de/acivates director mode in engine
#define DRCAPI_STATUS				2   	// request proxy information
#define DRCAPI_SETCAM				3		// set camera n to given position and angle
#define DRCAPI_GETCAM				4		// request camera n position and angle
#define DRCAPI_DIRPLAY				5		// set director time and play with normal speed
#define DRCAPI_DIRFREEZE			6		// freeze directo at this time
#define DRCAPI_SETVIEWMODE			7		// overview or 4 cameras 
#define DRCAPI_SETOVERVIEWPARAMS	8		// sets parameter for overview mode
#define DRCAPI_SETFOCUS				9		// set the camera which has the input focus
#define DRCAPI_GETTARGETS			10		// queries engine for player list
#define DRCAPI_SETVIEWPOINTS		11		// gives engine all waypoints

#endif