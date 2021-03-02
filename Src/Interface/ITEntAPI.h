#ifndef ITENTAPI_H
#define ITENTAPI_H

typedef struct tempentity_s
{
	int		valid;

	vec3_t	origin;
	void	*data;

	float	start;
	float	time;

	void	( *draw )	( tempentity_s *pEntity );
}
tempentity_t;

typedef struct tent_api_s
{
	tempentity_s *	( *AllocateEntity )		( void );
	void			( *DestroyEntity )		( tempentity_s *pEntity );
	void			( *DestroyAllEntity )	( void );
}
tent_api_t;

extern tent_api_t gTEntAPI;

#endif