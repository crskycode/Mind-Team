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

#ifndef CLIENTSTATE_H
#define CLIENTSTATE_H

typedef struct client_state_s
{
	int iRandomSeed;	// 用于构成武器弹道随机值的种子
	bool bIsAlive;		// 当前是否Alive
	int iFOV;			// 当前视景FOV值
	int iArmor;			// 当前AC值
	int iHealth;		// 当前HP值
	int iCharacterId;	// 当前角色的ID
	int iWeaponId;		// 当前持着的武器ID（如果没有武器ID为0）
	int iClip;			// 当前武器弹夹子弹剩余数量
	int iAmmo;			// 当前武器后备子弹剩余数量
	int iTeam;
	bool bEanbleBag;	// 当前是否启用背包
	bool bDucking;
	int iFlags;			// 等于pev->flags
	int iUser1;			// 玩家视角模式，用于观战状态（Alive状态下保持为0）
	int iUser2;			// 观战模式下观察的对象（玩家ID）
	int iUser3;			// 杀死自己的人的ID（用于死后将摄像机朝向他）
	vec3_t vecOrigin;	// 当前坐标
	vec3_t vecAngles;	// 当前角度
	vec3_t vecVelocity;	// 当前移动速度
}
client_state_t;

typedef struct view_model_s
{
	int iTeam;
	int iCharacterId;
	int iWeaponId;
}
view_model_t;

extern client_state_t ClientState;
extern view_model_t ViewModel;

#endif