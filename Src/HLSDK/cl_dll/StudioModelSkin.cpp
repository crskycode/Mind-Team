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
#include "MetaHook_Interface.h"
#include "qgl.h"

#include "ClientState.h"
#include "CharacterData.h"
#include "WeaponData.h"

#include <studio.h>


int StudioSetupSkin(cl_entity_t *entity, studiohdr_t *ptexturehdr, int index)
{
	mstudiotexture_t *ptexture = (mstudiotexture_t *)((byte *)ptexturehdr + ptexturehdr->textureindex) + index;

	if (entity->player)
	{
		studiohdr_t *pstudiohdr = (studiohdr_t *)*(DWORD *)0x024849C0;

		if (!entity->curstate.skin)		// WeaponID
			return 0;

		// Just PModel
		if (strnicmp(pstudiohdr->name, "QV-", 3) && strnicmp(pstudiohdr->name, "GV-", 3))
			return 0;

		WeaponData *pData = WeaponData_GetData(entity->curstate.skin);

		if (!pData)
			return 0;

		if (index == 0 && pData->szGViewSkinFileName[0])
		{
			tex_t *pReplace = gTexAPI.LoadDTX(pData->szGViewSkinFileName);

			if (!pReplace->id)
				return 0;

			qglBindTexture(GL_TEXTURE_2D, pReplace->id);
			return 1;
		}
		else if (index == 1 && pData->szGViewSkinFileName2[0])
		{
			tex_t *pReplace = gTexAPI.LoadDTX(pData->szGViewSkinFileName);

			if (!pReplace->id)
				return 0;

			qglBindTexture(GL_TEXTURE_2D, pReplace->id);
			return 1;
		}
	}
	else if (entity == gEngfuncs.GetViewModel())
	{
		if (!ViewModel.iWeaponId)
			return 0;

		if (index < 8)
		{
			CharacterData *pCharacterData = CharacterData_GetData(ViewModel.iCharacterId);

			if (pCharacterData)
			{
				if (!strnicmp(ptexture->name, "FVIEW_ARM_GR", 12) || !strnicmp(ptexture->name, "FVIEW_ARM_WOMAN_GR", 18))
				{
					if (pCharacterData->szGRPVArmTextureFile[0])
					{
						tex_t *texture = gTexAPI.LoadDTX(pCharacterData->szGRPVArmTextureFile);

						if (!texture->id)
							return 0;

						qglBindTexture(GL_TEXTURE_2D, texture->id);
						return 1;
					}
				}
				else if (!strnicmp(ptexture->name, "FVIEW_ARM_BL", 12) || !strnicmp(ptexture->name, "FVIEW_ARM_WOMAN_BL", 18))
				{
					if (pCharacterData->szBLPVArmTextureFile[0])
					{
						tex_t *texture = gTexAPI.LoadDTX(pCharacterData->szBLPVArmTextureFile);

						if (!texture->id)
							return 0;

						qglBindTexture(GL_TEXTURE_2D, texture->id);
						return 1;
					}
				}
				else if (!strnicmp(ptexture->name, "FVIEW_HAND_GR", 13) || !strnicmp(ptexture->name, "FVIEW_HAND_WOMAN_GR", 19))
				{
					if (pCharacterData->szGRPVHandTextureFile[0])
					{
						tex_t *texture = gTexAPI.LoadDTX(pCharacterData->szGRPVHandTextureFile);

						if (!texture->id)
							return 0;

						qglBindTexture(GL_TEXTURE_2D, texture->id);
						return 1;
					}
				}
				else if (!strnicmp(ptexture->name, "FVIEW_HAND_BL", 13) || !strnicmp(ptexture->name, "FVIEW_HAND_WOMAN_BL", 19))
				{
					if (pCharacterData->szBLPVHandTextureFile[0])
					{
						tex_t *texture = gTexAPI.LoadDTX(pCharacterData->szBLPVHandTextureFile);

						if (!texture->id)
							return 0;

						qglBindTexture(GL_TEXTURE_2D, texture->id);
						return 1;
					}
				}
			}
		}
		else
		{
			WeaponData *pWeaponData = WeaponData_GetData(ViewModel.iWeaponId);

			if (pWeaponData)
			{
				if (index == 8 && pWeaponData->szPViewSkinFileName[0])
				{
					tex_t *texture = gTexAPI.LoadDTX(pWeaponData->szPViewSkinFileName);

					if (!texture->id)
						return 0;

					qglBindTexture(GL_TEXTURE_2D, texture->id);
					return 1;
				}
				else if (index == 9 && pWeaponData->szPViewSkinFileName2[0])
				{
					tex_t *texture = gTexAPI.LoadDTX(pWeaponData->szPViewSkinFileName2);

					if (!texture->id)
						return 0;

					qglBindTexture(GL_TEXTURE_2D, texture->id);
					return 1;
				}
			}
		}
	}
	else if (entity->curstate.iuser1 == 101)	// WeaponBox Identity
	{
		if (!entity->curstate.iuser2)	// WeaponID
			return 0;

		WeaponData *pData = WeaponData_GetData(entity->curstate.iuser2);

		if (!pData)
			return 0;

		if (index == 0 && pData->szSkinFileName[0])
		{
			tex_t *texture = gTexAPI.LoadDTX(pData->szSkinFileName);

			if (!texture->id)
				return 0;

			qglBindTexture(GL_TEXTURE_2D, texture->id);
			return 1;
		}
		else if (index == 1 && pData->szSkinFileName2[0])
		{
			tex_t *texture = gTexAPI.LoadDTX(pData->szSkinFileName2);

			if (!texture->id)
				return 0;

			qglBindTexture(GL_TEXTURE_2D, texture->id);
			return 1;
		}
	}

	return 0;
}

void StudioModelSkin_Init(void)
{
	gpMTEngine->SetModelSkinCallBack( (STUDIOSETUPSKINPROC)&StudioSetupSkin );
}