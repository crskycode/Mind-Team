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

#ifndef FUNC_BREAK_H
#define FUNC_BREAK_H

enum Explosions
{
	expRandom,
	expDirected,
};

enum Materials
{
	matGlass,
	matWood,
	matMetal,
	matFlesh,
	matCinderBlock,
	matCeilingTile,
	matComputer,
	matUnbreakableGlass,
	matRocks,
	matNone,
	matLastMaterial,
};


#define SF_TRIGGER_ALLOWMONSTERS	(1<<0)
#define SF_TRIGGER_NOCLIENTS		(1<<1)
#define SF_TRIGGER_PUSHABLES		(1<<2)

#define SF_BREAK_TRIGGER_ONLY	(1<<0)
#define	SF_BREAK_TOUCH			(1<<1)
#define SF_BREAK_PRESSURE		(1<<2)
#define SF_BREAK_CROWBAR		(1<<8)

#define SF_PUSH_BREAKABLE		(1<<7)

class CBreakable : public CBaseDelay
{
public:
	virtual void Spawn( void );
	virtual void Precache( void );
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	virtual int ObjectCaps( void )
	{
		return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION;
	}

	void BreakTouch( CBaseEntity *pOther );
	void DamageSound( void );

	virtual void TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	virtual void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector &vecDir, TraceResult *ptr, int bitsDamageType );

	BOOL IsBreakable( void );
	BOOL SparkWhenHit( void );

	int DamageDecal( int bitsDamageType );

	void Die( void );

	inline BOOL Explodable( void ) { return ExplosionMagnitude() > 0; }
	inline int ExplosionMagnitude( void ) { return pev->impulse; }
	inline void	 ExplosionSetMagnitude( int magnitude ) { pev->impulse = magnitude; }

	static void MaterialSoundPrecache( Materials precacheMaterial );
	static void MaterialSoundRandom( edict_t *pEdict, Materials soundMaterial, float volume );
	static const char **MaterialSoundList( Materials precacheMaterial, int &soundCount );

	static const char *pSoundsWood[];
	static const char *pSoundsFlesh[];
	static const char *pSoundsGlass[];
	static const char *pSoundsMetal[];
	static const char *pSoundsConcrete[];
	static const char *pSpawnObjects[];

	Materials m_Material;
	Explosions m_Explosion;
	int m_idShard;
	float m_angle;
	int m_iszGibModel;
	int m_iszSpawnObject;
};

#endif