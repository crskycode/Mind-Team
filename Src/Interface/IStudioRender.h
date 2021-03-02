#ifndef ISTUDIORENDER_H
#define ISTUDIORENDER_H

typedef void *HSTUDIO;

typedef struct studio_render_s
{
	HSTUDIO				( *Create )				( void );
	void				( *Release )			( HSTUDIO handle );
	void				( *Init )				( HSTUDIO handle, int width, int height );
	void				( *Load )				( HSTUDIO handle, const char *modelname );
	void				( *Free )				( HSTUDIO handle );
	void				( *Draw )				( HSTUDIO handle );
	void				( *AdvanceFrame )		( HSTUDIO handle, float dt );
	void				( *ExtractBbox )		( HSTUDIO handle, float *mins, float *maxs );
	int					( *SetSequence )		( HSTUDIO handle, int iSequence );
	int					( *GetSequence )		( HSTUDIO handle );
	void				( *GetSequenceInfo )	( HSTUDIO handle, float *pflFrameRate, float *pflGroundSpeed );
	float				( *SetController )		( HSTUDIO handle, int iController, float flValue );
	float				( *SetMouth )			( HSTUDIO handle, float flValue );
	float				( *SetBlending )		( HSTUDIO handle, int iBlender, float flValue );
	int					( *SetBodygroup )		( HSTUDIO handle, int iGroup, int iValue );
	int					( *SetSkin )			( HSTUDIO handle, int iValue );
	void				( *SetPerspective )		( HSTUDIO handle, float fovy, float aspect, float zNear, float zFar );
	void				( *SetOrigin )			( HSTUDIO handle, float x, float y, float z );
	void				( *SetAngles )			( HSTUDIO handle, float x, float y, float z );
	void				( *SetScale )			( HSTUDIO handle, float s );
	int					( *GetColorBuffer )		( HSTUDIO handle );
}
studio_render_t;

extern studio_render_t gStudioRender;

#endif