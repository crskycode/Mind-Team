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
	int iRandomSeed;	// ���ڹ��������������ֵ������
	bool bIsAlive;		// ��ǰ�Ƿ�Alive
	int iFOV;			// ��ǰ�Ӿ�FOVֵ
	int iArmor;			// ��ǰACֵ
	int iHealth;		// ��ǰHPֵ
	int iCharacterId;	// ��ǰ��ɫ��ID
	int iWeaponId;		// ��ǰ���ŵ�����ID�����û������IDΪ0��
	int iClip;			// ��ǰ���������ӵ�ʣ������
	int iAmmo;			// ��ǰ�������ӵ�ʣ������
	int iTeam;
	bool bEanbleBag;	// ��ǰ�Ƿ����ñ���
	bool bDucking;
	int iFlags;			// ����pev->flags
	int iUser1;			// ����ӽ�ģʽ�����ڹ�ս״̬��Alive״̬�±���Ϊ0��
	int iUser2;			// ��սģʽ�¹۲�Ķ������ID��
	int iUser3;			// ɱ���Լ����˵�ID�����������������������
	vec3_t vecOrigin;	// ��ǰ����
	vec3_t vecAngles;	// ��ǰ�Ƕ�
	vec3_t vecVelocity;	// ��ǰ�ƶ��ٶ�
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