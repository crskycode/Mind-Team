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

#ifndef CS_BOT_MANAGER_H
#define CS_BOT_MANAGER_H
extern CBotManager *TheBots;

// The manager for Counter-Strike specific bots
class CCSBotManager: public CBotManager
{
public:
	CCSBotManager();

	virtual void ClientDisconnect(CBasePlayer *pPlayer);
	virtual BOOL ClientCommand(CBasePlayer *pPlayer, const char *pcmd);

	virtual void ServerActivate();
	virtual void ServerDeactivate();

	virtual void ServerCommand(const char *pcmd);
	virtual void AddServerCommand(const char *cmd);
	virtual void AddServerCommands();

	virtual void RestartRound();											// (EXTEND) invoked when a new round begins
	virtual void StartFrame();												// (EXTEND) called each frame

	virtual void OnEvent(GameEventType event, CBaseEntity *entity = NULL, CBaseEntity *other = NULL);
	virtual unsigned int GetPlayerPriority(CBasePlayer *player) const;		// return priority of player (0 = max pri)
	virtual bool IsImportantPlayer(CBasePlayer *player) const;				// return true if player is important to scenario (VIP, bomb carrier, etc)

public:
	void ValidateMapData();
	void OnFreeEntPrivateData(CBaseEntity *pEntity);
	bool IsLearningMap() const { return m_isLearningMap; }
	void SetLearningMapFlag() { m_isLearningMap = true; }
	bool IsAnalysisRequested() const { return m_isAnalysisRequested; }
	void RequestAnalysis() { m_isAnalysisRequested = true; }
	void AckAnalysisRequest() { m_isAnalysisRequested = false; }

	// difficulty levels
	static BotDifficultyType GetDifficultyLevel()
	{
		if (cv_bot_difficulty.value < 0.9f)
			return BOT_EASY;

		if (cv_bot_difficulty.value < 1.9f)
			return BOT_NORMAL;

		if (cv_bot_difficulty.value < 2.9f)
			return BOT_HARD;

		return BOT_EXPERT;
	}

	// the supported game scenarios
	enum GameScenarioType
	{
		SCENARIO_DEATHMATCH,
		SCENARIO_DEFUSE_BOMB,
		SCENARIO_RESCUE_HOSTAGES,
		SCENARIO_ESCORT_VIP
	};
	GameScenarioType GetScenario() const
	{
#ifdef REGAMEDLL_ADD
		// if we have included deathmatch mode, so set the game type like SCENARIO_DEATHMATCH
		if (cv_bot_deathmatch.value > 0)
			return SCENARIO_DEATHMATCH;
#endif

		return m_gameScenario;
	}

	// "zones"
	// depending on the game mode, these are bomb zones, rescue zones, etc.
	enum { MAX_ZONES = 4 };						// max # of zones in a map
	enum { MAX_ZONE_NAV_AREAS = 16 };			// max # of nav areas in a zone
	struct Zone
	{
		CBaseEntity *m_entity;					// the map entity
		CNavArea *m_area[MAX_ZONE_NAV_AREAS];	// nav areas that overlap this zone
		int m_areaCount;
		Vector m_center;
		bool m_isLegacy;						// if true, use pev->origin and 256 unit radius as zone
		int m_index;
		Extent m_extent;
	};

	const Zone *GetZone(int i) const { return &m_zone[i]; }
	const Zone *GetZone(const Vector *pos) const;										// return the zone that contains the given position
	const Zone *GetClosestZone(const Vector *pos) const;								// return the closest zone to the given position
	const Zone *GetClosestZone(const CBaseEntity *entity) const { return GetClosestZone(&entity->pev->origin); }		// return the closest zone to the given entity
	int GetZoneCount() const { return m_zoneCount; }

	const Vector *GetRandomPositionInZone(const Zone *zone) const;
	CNavArea *GetRandomAreaInZone(const Zone *zone) const;

	// Return the zone closest to the given position, using the given cost heuristic
	template<typename CostFunctor>
	const Zone *GetClosestZone(CNavArea *startArea, CostFunctor costFunc, float *travelDistance = nullptr) const
	{
		const Zone *closeZone = nullptr;
		float closeDist = 99999999.9f;

		if (startArea == nullptr)
			return nullptr;

		for (int i = 0; i < m_zoneCount; ++i)
		{
			if (m_zone[i].m_areaCount == 0)
				continue;

			// just use the first overlapping nav area as a reasonable approximation
			float_precision dist = NavAreaTravelDistance(startArea, m_zone[i].m_area[0], costFunc);

			if (/*dist >= 0.0f && */dist < closeDist)
			{
				closeZone = &m_zone[i];
				closeDist = dist;
			}
		}

		if (travelDistance)
			*travelDistance = closeDist;

		return closeZone;
	}

	// pick a zone at random and return it
	const Zone *GetRandomZone() const
	{
		if (!m_zoneCount)
			return NULL;

		return &m_zone[RANDOM_LONG(0, m_zoneCount - 1)];
	}

	bool IsBombPlanted() const { return m_isBombPlanted; }											// returns true if bomb has been planted
	float GetBombPlantTimestamp() const { return m_bombPlantTimestamp; }							// return time bomb was planted
	bool IsTimeToPlantBomb() const { return (gpGlobals->time >= m_earliestBombPlantTimestamp); }	// return true if it's ok to try to plant bomb
	CBasePlayer *GetBombDefuser() const { return m_bombDefuser; }									// return the player currently defusing the bomb, or NULL
	float GetBombTimeLeft() const;																	// get the time remaining before the planted bomb explodes
	CBaseEntity *GetLooseBomb() { return m_looseBomb; }												// return the bomb if it is loose on the ground
	CNavArea *GetLooseBombArea() const { return m_looseBombArea; }									// return area that bomb is in/near
	void SetLooseBomb(CBaseEntity *bomb);

//	float GetRadioMessageTimestamp(GameEventType event, int teamID) const;					// return the last time the given radio message was sent for given team
//	float GetRadioMessageInterval(GameEventType event, int teamID) const;					// return the interval since the last time this message was sent
//	void SetRadioMessageTimestamp(GameEventType event, int teamID);
//	void ResetRadioMessageTimestamps();

	float GetLastSeenEnemyTimestamp() const { return m_lastSeenEnemyTimestamp; }			// return the last time anyone has seen an enemy
	void SetLastSeenEnemyTimestamp() { m_lastSeenEnemyTimestamp = gpGlobals->time; }

	float GetRoundStartTime() const { return m_roundStartTimestamp; }
	float GetElapsedRoundTime() const { return gpGlobals->time - m_roundStartTimestamp; }		// return the elapsed time since the current round began

	bool AllowRogues() const { return cv_bot_allow_rogues.value != 0.0f; }
	bool AllowPistols() const { return cv_bot_allow_pistols.value != 0.0f; }
	bool AllowShotguns() const { return cv_bot_allow_shotguns.value != 0.0f; }
	bool AllowSubMachineGuns() const { return cv_bot_allow_sub_machine_guns.value != 0.0f; }
	bool AllowRifles() const { return cv_bot_allow_rifles.value != 0.0f; }
	bool AllowMachineGuns() const { return cv_bot_allow_machine_guns.value != 0.0f; }
	bool AllowGrenades() const { return cv_bot_allow_grenades.value != 0.0f; }
	bool AllowSnipers() const { return cv_bot_allow_snipers.value != 0.0f; }
	bool AllowTacticalShield() const { return cv_bot_allow_shield.value != 0.0f; }
	bool AllowFriendlyFireDamage() const { return false; }

	bool IsWeaponUseable(CBasePlayerWeapon *item) const;						// return true if the bot can use this weapon

	bool IsDefenseRushing() const { return m_isDefenseRushing; }				// returns true if defense team has "decided" to rush this round
	bool IsOnDefense(CBasePlayer *player) const;							// return true if this player is on "defense"
	bool IsOnOffense(CBasePlayer *player) const;							// return true if this player is on "offense"

	bool IsRoundOver() const { return m_isRoundOver; }						// return true if the round has ended

	unsigned int GetNavPlace() const { return m_navPlace; }
	void SetNavPlace(unsigned int place) { m_navPlace = place; }

	enum SkillType { LOW, AVERAGE, HIGH, RANDOM };
	const char *GetRandomBotName(SkillType skill);

	void MonitorBotCVars();
	void MaintainBotQuota();
	bool AddBot(const BotProfile *profile, BotProfileTeamType team);

	#define FROM_CONSOLE true
	bool BotAddCommand(BotProfileTeamType team, bool isFromConsole = false);	// process the "bot_add" console command

private:
	static float m_flNextCVarCheck;
	static bool m_isMapDataLoaded;		// true if we've attempted to load map data
	static bool m_isLearningMap;
	static bool m_isAnalysisRequested;

	GameScenarioType m_gameScenario;			// what kind of game are we playing

	Zone m_zone[MAX_ZONES];
	int m_zoneCount;

	bool m_isBombPlanted;						// true if bomb has been planted
	float m_bombPlantTimestamp;					// time bomb was planted
	float m_earliestBombPlantTimestamp;			// don't allow planting until after this time has elapsed
	CBasePlayer *m_bombDefuser;					// the player currently defusing a bomb
	EHANDLE m_looseBomb;						// will be non-NULL if bomb is loose on the ground
	CNavArea *m_looseBombArea;					// area that bomb is is/near

	bool m_isRoundOver;							// true if the round has ended

	float m_radioMsgTimestamp[24][2];

	float m_lastSeenEnemyTimestamp;
	float m_roundStartTimestamp;				// the time when the current round began

	bool m_isDefenseRushing;					// whether defensive team is rushing this round or not

	static NavEditCmdType m_editCmd;
	unsigned int m_navPlace;
	CountdownTimer m_respawnTimer;
	bool m_isRespawnStarted;
	bool m_canRespawn;
	bool m_bServerActive;
};

inline int OtherTeam(int team)
{
	return (team == 1) ? 2 : 1;
}


extern CCSBotManager *TheCSBots;

// Determine whether bots can be used or not
inline bool AreBotsAllowed()
{
//	return g_bAllowedCSBot;
	return true;
}


#endif // CS_BOT_MANAGER_H
