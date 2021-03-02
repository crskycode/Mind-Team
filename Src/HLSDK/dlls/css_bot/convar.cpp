
#include "extdll.h"
#include "util.h"
#include "convar.h"

#pragma init_seg( lib )
ConCommand *g_pCmdRegLink = NULL;
ConVar *g_pCvarRegLink = NULL;


ConCommand::ConCommand( const char *pName, FnCommandCallbackV1_t callback, const char *pHelpString, int flags )
{
	m_name = pName;
	m_callback = callback;

	m_registered = false;

	m_next = g_pCmdRegLink;
	g_pCmdRegLink = this;
}

ConVar::ConVar( const char *pName, const char *pDefaultValue, int flags, const char *pHelpString )
{
	m_value.name = (char *)pName;
	m_value.string = (char *)pDefaultValue;
	m_value.flags = flags;
	m_value.value = 0;
	m_value.next = 0;

	m_registered = false;

	m_next = g_pCvarRegLink;
	g_pCvarRegLink = this;
}

float ConVar::GetFloat( void )
{
	return m_value.value;
}

int ConVar::GetInt( void )
{
	return (int)m_value.value;
}

bool ConVar::GetBool( void )
{
	return m_value.value > 0.0f;
}

const char *ConVar::GetString( void )
{
	return m_value.string;
}

void ConVar::SetValue( float value )
{
	CVAR_SET_FLOAT( m_value.name, value );
}

void ConVar::SetValue( int value )
{
	CVAR_SET_FLOAT( m_value.name, value );
}

void ConVar::SetValue( const char *value )
{
	CVAR_SET_STRING( m_value.name, value );
}

//--------------------------------------------------------------
void ConVar_Register( void )
{
	ConCommand *cmd = g_pCmdRegLink;

	while ( cmd )
	{
		if ( !cmd->m_registered )
		{
			ADD_SERVER_COMMAND( cmd->m_name, cmd->m_callback );
			cmd->m_registered = true;
		}

		cmd = cmd->m_next;
	}

	ConVar *cvar = g_pCvarRegLink;

	while ( cvar )
	{
		if ( !cvar->m_registered )
		{
			CVAR_REGISTER( &cvar->m_value );
			cvar->m_registered = true;
		}

		cvar = cvar->m_next;
	}
}