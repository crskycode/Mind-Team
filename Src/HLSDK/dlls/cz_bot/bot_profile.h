/****
 * 
 * Copyright (c) 2017, Crsky
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
****/

#ifndef BOT_PROFILE_H
#define BOT_PROFILE_H

#include <list>

enum BotProfileTeamType
{
	BOT_TEAM_T,
	BOT_TEAM_CT,
	BOT_TEAM_ANY,
};

class BotProfile
{
public:
	BotProfile();
	
	BotProfileTeamType GetTeamType(void) const;
	const char *GetBotName(void) const;
	float GetAggression(void) const;
	float GetSkill(void) const;
	float GetTeamwork(void) const;
	float GetReactionTime(void) const;
	float GetAttackDelay(void) const;

	const char *GetName(void) const { return GetBotName(); }
	
private:
	BotProfileTeamType m_iTeamType;
	char m_szProfileName[64];
	char m_szBotName[64];
	float m_flAggression;
	float m_flSkill;
	float m_flTeamwork;
	float m_flReactionTime;
	float m_flAttackDelay;
};

typedef std::list<BotProfile *> BotProfileList;

class BotProfileManager
{
public:
	BotProfileManager();
	~BotProfileManager();

	void Init(const char *filename);
	
	BotProfile *GetProfile(const char *szName);
	
private:
	BotProfileList m_ProfileList;
};

extern BotProfileManager *TheBotProfiles;

#endif