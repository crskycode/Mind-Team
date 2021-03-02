
#ifndef ENGFUNCS_H
#define ENGFUNCS_H

void EngFuncs_Init(void);

// engine module version
extern DWORD g_dwEngineBuildnum;

// engine module address space
extern void* g_dwEngineBase;
extern DWORD g_dwEngineSize;



void Sys_Error(char *error, ...);

// engine variable pointer
extern int*		g_pfn_cl_viewentity;
extern int*		g_pfn_cl_parsecount;
extern void*	g_pfn_rgrgvoxword;

//#define cl_servercount	( *(int *)0x02DE0FFC )
#define cl_viewentity	( *g_pfn_cl_viewentity )
#define cl_parsecount	( *g_pfn_cl_parsecount )


// function prototype
typedef void				(*PFN_S_STARTUP_T)				(void);
typedef void				(*PFN_S_INIT_T)					(void);
typedef void				(*PFN_S_SHUTDOWN_T)				(void);
typedef struct sfx_s *		(*PFN_S_FINDNAME_T)				(char *, qboolean *);
typedef void				(*PFN_S_STARTDYNAMICSOUND_T)	(int, int, struct sfx_s *, vec3_t, float, float, int, int);
typedef void				(*PFN_S_STARTSTATICSOUND_T)		(int, int, struct sfx_s *, vec3_t, float, float, int, int);
typedef void				(*PFN_S_STOPSOUND_T)			(int, int);
typedef void				(*PFN_S_STOPALLSOUNDS_T)		(qboolean);
typedef void				(*PFN_S_UPDATE_T)				(vec3_t, vec3_t, vec3_t, vec3_t);
typedef struct sfxcache_s *	(*PFN_S_LOADSOUND_T)			(struct sfx_s *, struct channel_s *);
typedef void				(*PFN_S_FREECHANNEL_T)			(struct channel_s *);
typedef struct sfxcache_s *	(*PFN_VOX_LOADSOUND_T)			(struct channel_s *, char *);
typedef void *				(*PFN_CACHE_CHECK_T)			(struct cache_user_s *);
typedef void *				(*PFN_CACHE_ALLOC_T)			(struct cache_user_s *, int, char *);
typedef void				(*PFN_CACHE_FREE_T)				(struct cache_user_s *);


// function callback
extern PFN_S_STARTUP_T				g_pfn_S_Startup;
extern PFN_S_INIT_T					g_pfn_S_Init;
extern PFN_S_SHUTDOWN_T				g_pfn_S_Shutdown;
extern PFN_S_FINDNAME_T				g_pfn_S_FindName;
extern PFN_S_STARTDYNAMICSOUND_T	g_pfn_S_StartDynamicSound;
extern PFN_S_STARTSTATICSOUND_T		g_pfn_S_StartStaticSound;
extern PFN_S_STOPSOUND_T			g_pfn_S_StopSound;
extern PFN_S_STOPALLSOUNDS_T		g_pfn_S_StopAllSounds;
extern PFN_S_UPDATE_T				g_pfn_S_Update;
extern PFN_S_LOADSOUND_T			g_pfn_S_LoadSound;
extern PFN_S_FREECHANNEL_T			g_pfn_S_FreeChannel;
extern PFN_VOX_LOADSOUND_T			g_pfn_VOX_LoadSound;
extern PFN_CACHE_CHECK_T			g_pfn_Cache_Check;
extern PFN_CACHE_ALLOC_T			g_pfn_Cache_Alloc;
extern PFN_CACHE_FREE_T				g_pfn_Cache_Free;


// hooked functions
void				S_Startup			(void);
void				S_Init				(void);
void				S_Shutdown			(void);
struct sfx_s *		S_FindName			(char *name, qboolean *pfInCache);
void				S_StartDynamicSound	(int entnum, int entchannel, struct sfx_s *sfx, vec3_t origin, float fvol, float attenuation, int flags, int pitch);
void				S_StartStaticSound	(int entnum, int entchannel, struct sfx_s *sfx, vec3_t origin, float fvol, float attenuation, int flags, int pitch);
void				S_StopSound			(int entnum, int entchannel);
void				S_StopAllSounds		(qboolean clear);
void				S_Update			(vec3_t origin, vec3_t forward, vec3_t right, vec3_t up);
struct sfxcache_s *	S_LoadSound			(struct sfx_s *s, struct channel_s *ch);
void				S_FreeChannel		(struct channel_s *ch);

#endif