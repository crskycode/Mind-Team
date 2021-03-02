/***
 * 
 * Copyright(c) 1996-2002, Valve LLC. All rights reserved.
 * 
 * This product contains software technology licensed from Id
 * Software, Inc.("Id Technology"). Id Technology(c) 1996 Id Software, Inc.
 * All Rights Reserved.
 * 
 * Use, distribution, and modification of this source code and/or resulting
 * object code is restricted to non-commercial enhancements to products from
 * Valve LLC. All other use, distribution, or modification is prohibited
 * without written permission from Valve LLC.
 * 
***/

#ifndef CLIENT_H
#define CLIENT_H

BOOL ClientConnect(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
void ClientDisconnect(edict_t *pEntity);
void ClientKill(edict_t *pEntity);
void ClientPutInServer(edict_t *pEntity);
void ClientCommand(edict_t *pEntity);
void ClientUserInfoChanged(edict_t *pEntity, char *infobuffer);
void ServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
void ServerDeactivate(void);
void PlayerPreThink(edict_t *pEntity);
void PlayerPostThink(edict_t *pEntity);
void StartFrame(void);
void ParmsNewLevel(void);
void ParmsChangeLevel(void);
const char *GetGameDescription(void);
void PlayerCustomization(edict_t *pEntity, struct customization_s *pCust);
void SpectatorConnect(edict_t *pEntity);
void SpectatorDisconnect(edict_t *pEntity);
void SpectatorThink(edict_t *pEntity);
void Sys_Error(const char *error_string);
void SetupVisibility(edict_t *pViewEntity, edict_t *pClient, unsigned char **pvs, unsigned char **pas);
void UpdateClientData(const struct edict_s *ent, int sendweapons, struct clientdata_s *cd);
int AddToFullPack(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet);
void CreateBaseline(int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs);
void RegisterEncoders(void);
int GetWeaponData(struct edict_s *player, struct weapon_data_s *info);
void CmdStart(const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed);
void CmdEnd(const edict_t *player);
int ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size);
int GetHullBounds(int hullnumber, float *mins, float *maxs);
void CreateInstancedBaselines(void);
int InconsistentFile(const edict_t *player, const char *filename, char *disconnect_message);
int AllowLagCompensation(void);

#endif