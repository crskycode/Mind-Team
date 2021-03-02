#include "cl_dll.h"
#include "hud.h"
#include "CharacterData.h"

void CHudCharacter::VidInit(void)
{
}

void CHudCharacter::Redraw(void)
{
	if (!ClientState.bIsAlive)
		return;

	CharacterData *pData = CharacterData_GetData(ClientState.iCharacterId);

	if (!pData)
		return;

	tex_t *pEffectTexture, *pLineTexture;

	pEffectTexture	= gTexAPI.LoadDTX(pData->szHUDEffectPath);
	pLineTexture	= gTexAPI.LoadDTX(pData->szHUDLinePath);

	int iBaseX = 0;
	int iBaseY = ScreenTall - 87;

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetTexture(pEffectTexture);
	gTexAPI.DrawSetColor(29, 42, 44, 120);
	gTexAPI.DrawTexturedRect(iBaseX, iBaseY, 128, 128);

	// бли╚ 64 153 92

	// calc flash
	int alpha = sin( ClientTime * 40 ) * 127 + 127;

	gTexAPI.RenderMode(RenderNormal);
	gTexAPI.DrawSetTexture(pEffectTexture);
	gTexAPI.DrawSetColor(64, 153, 92, 200);
	gTexAPI.DrawTexturedRect(iBaseX, iBaseY, 128, 128);

	gTexAPI.RenderMode(RenderAdditive);
	gTexAPI.DrawSetTexture(pLineTexture);
	gTexAPI.DrawSetColor(255, 255, 255, 255);
	gTexAPI.DrawTexturedRect(iBaseX, iBaseY, 128, 128);
}

void CHudCharacter::Save(CBufferWriter &buf)
{
	buf.WriteInt(20);

	gEngfuncs.Con_Printf("CHudCharacter: Save()\n");
}

void CHudCharacter::Restore(CBufferReader &buf)
{
	int data = buf.ReadInt();

	gEngfuncs.Con_Printf("Data %d\n", data);
}