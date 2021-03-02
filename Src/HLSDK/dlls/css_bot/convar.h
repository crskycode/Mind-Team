
#ifndef CONVAR_H
#define CONVAR_H

#include "cvardef.h"

typedef void ( *FnCommandCallbackV1_t )( void );

class ConCommand
{
public:
	ConCommand( const char *pName, FnCommandCallbackV1_t callback, const char *pHelpString = 0, int flags = 0 );

public:
	const char				*m_name;
	FnCommandCallbackV1_t	m_callback;
	bool					m_registered;
	ConCommand				*m_next;
};

class ConVar
{
public:
	ConVar( const char *pName, const char *pDefaultValue, int flags = 0, const char *pHelpString = 0 );

	float	GetFloat( void );
	int		GetInt( void );
	bool	GetBool( void );
	const char * GetString( void );

	void	SetValue( float value );
	void	SetValue( int value );
	void	SetValue( const char *value );

public:
	cvar_t		m_value;
	bool		m_registered;
	ConVar		*m_next;
};

void ConVar_Register( void );

#define CON_COMMAND_F( name, description ) \
   static void name( void ); \
   static ConCommand name##_command( #name, name, description ); \
   static void name( void )

#endif	// CONVAR_H
