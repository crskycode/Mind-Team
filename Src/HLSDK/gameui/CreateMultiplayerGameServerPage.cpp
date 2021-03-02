//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#include "CreateMultiplayerGameServerPage.h"

using namespace vgui;

#include <KeyValues.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/RadioButton.h>
#include <vgui_controls/CheckButton.h>
#include "FileSystem.h"
#include "EngineInterface.h"
#include "CvarToggleCheckButton.h"


// for SRC

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

#define RANDOM_MAP "#GameUI_RandomMap"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CCreateMultiplayerGameServerPage::CCreateMultiplayerGameServerPage(vgui::Panel *parent, const char *name) : PropertyPage(parent, name)
{
	m_pSavedData = NULL;

	// we can use this if we decide we want to put "listen server" at the end of the game name
	m_pMapList = new ComboBox(this, "MapList", 12, false);

	m_pEnableBotsCheck = new CheckButton( this, "EnableBotsCheck", "" );
	m_pEnableBotsCheck->SetVisible( false );
	m_pEnableBotsCheck->SetEnabled( false );

	LoadControlSettings("Resource/CreateMultiplayerGameServerPage.res");

	LoadMapList();
	m_szMapName[0]  = 0;

	// initialize hostname

	// initialize password
	SetControlString("PasswordEdit", engine->pfnGetCvarString("sv_password"));
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CCreateMultiplayerGameServerPage::~CCreateMultiplayerGameServerPage()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameServerPage::EnableBots( KeyValues *data )
{
	m_pSavedData = data;

	int quota = data->GetInt( "bot_quota", 0 );
	SetControlInt( "BotQuotaCombo", quota );
	m_pEnableBotsCheck->SetSelected( (quota > 0) );

	int difficulty = data->GetInt( "bot_difficulty", 0 );
	difficulty = max( difficulty, 0 );
	difficulty = min( 3, difficulty );

	char buttonName[64];
	Q_snprintf( buttonName, sizeof( buttonName ), "SkillLevel%d", difficulty );
	vgui::RadioButton *button = dynamic_cast< vgui::RadioButton * >(FindChildByName( buttonName ));
	if ( button )
	{
		button->SetSelected( true );
	}
}

//-----------------------------------------------------------------------------
// Purpose: called to get the info from the dialog
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameServerPage::OnApplyChanges()
{
	KeyValues *kv = m_pMapList->GetActiveItemUserData();
	strncpy(m_szMapName, kv->GetString("mapname", ""), DATA_STR_LENGTH);

	if ( m_pSavedData )
	{
		int quota = GetControlInt( "BotQuotaCombo", 0 );
		if ( !m_pEnableBotsCheck->IsSelected() )
		{
			quota = 0;
		}
		m_pSavedData->SetInt( "bot_quota", quota );
		engine->Cvar_SetValue( "bot_quota", quota );

		int difficulty = 0;
		for ( int i=0; i<4; ++i )
		{
			char buttonName[64];
			Q_snprintf( buttonName, sizeof( buttonName ), "SkillLevel%d", i );
			vgui::RadioButton *button = dynamic_cast< vgui::RadioButton * >(FindChildByName( buttonName ));
			if ( button )
			{
				if ( button->IsSelected() )
				{
					difficulty = i;
					break;
				}
			}
		}
		m_pSavedData->SetInt( "bot_difficulty", difficulty );
		engine->Cvar_SetValue( "bot_difficulty", difficulty );
	}
}

//-----------------------------------------------------------------------------
// Purpose: loads the list of available maps into the map list
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameServerPage::LoadMaps( const char *pszPathID )
{
	FileFindHandle_t findHandle = NULL;


	const char *pszFilename = g_pFullFileSystem->FindFirst( "maps/*.bsp", &findHandle, pszPathID );
	while ( pszFilename )
	{
		char mapname[256];

		// FindFirst ignores the pszPathID, so check it here
		// TODO: this doesn't find maps in fallback dirs
		_snprintf( mapname, sizeof(mapname), "maps/%s", pszFilename );
		if ( !g_pFullFileSystem->FileExists( mapname ) )
		{
			goto nextFile;
		}

		// remove the text 'maps/' and '.bsp' from the file name to get the map name
		
		const char *str = Q_strstr( pszFilename, "maps" );
		if ( str )
		{
			strncpy( mapname, str + 5, sizeof(mapname) - 1 );	// maps + \\ = 5
		}
		else
		{
			strncpy( mapname, pszFilename, sizeof(mapname) - 1 );
		}
		char *ext = Q_strstr( mapname, ".bsp" );
		if ( ext )
		{
			*ext = 0;
		}

		//!! hack: strip out single player HL maps
		// this needs to be specified in a seperate file

		// strip out maps that shouldn't be displayed

		// add to the map list
		m_pMapList->AddItem( mapname, new KeyValues( "data", "mapname", mapname ) );

		// get the next file
	nextFile:
		pszFilename = g_pFullFileSystem->FindNext( findHandle );
	}
	g_pFullFileSystem->FindClose( findHandle );
}



//-----------------------------------------------------------------------------
// Purpose: loads the list of available maps into the map list
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameServerPage::LoadMapList()
{
	// clear the current list (if any)
	m_pMapList->DeleteAllItems();

	// add special "name" to represent loading a randomly selected map
	m_pMapList->AddItem( RANDOM_MAP, new KeyValues( "data", "mapname", RANDOM_MAP ) );

	// iterate the filesystem getting the list of all the files
	// UNDONE: steam wants this done in a special way, need to support that
	const char *pathID = "GAME";

	// Load the GameDir maps
	LoadMaps( pathID ); 

	// If we're not the Valve directory and we're using a "fallback_dir" in gameinfo.txt then include those maps...
	// (pathID is NULL if we're "Half-Life")

	// set the first item to be selected
	m_pMapList->ActivateItem( 0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CCreateMultiplayerGameServerPage::IsRandomMapSelected()
{
	const char *mapname = m_pMapList->GetActiveItemUserData()->GetString("mapname");
	if (!stricmp( mapname, RANDOM_MAP ))
	{
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CCreateMultiplayerGameServerPage::GetMapName()
{
	int count = m_pMapList->GetItemCount();

	// if there is only one entry it's the special "select random map" entry
	if( count <= 1 )
		return NULL;

	const char *mapname = m_pMapList->GetActiveItemUserData()->GetString("mapname");
	if (!strcmp( mapname, RANDOM_MAP ))
	{
		int which = engine->pfnRandomLong( 1, count - 1 );
		mapname = m_pMapList->GetItemUserData( which )->GetString("mapname");
	}

	return mapname;
}

//-----------------------------------------------------------------------------
// Purpose: Sets currently selected map in the map combobox
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameServerPage::SetMap(const char *mapName)
{
	for (int i = 0; i < m_pMapList->GetItemCount(); i++)
	{
		if (!m_pMapList->IsItemIDValid(i))
			continue;

		if (!stricmp(m_pMapList->GetItemUserData(i)->GetString("mapname"), mapName))
		{
			m_pMapList->ActivateItem(i);
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameServerPage::OnCheckButtonChecked()
{
	SetControlEnabled("SkillLevel0", m_pEnableBotsCheck->IsSelected());
	SetControlEnabled("SkillLevel1", m_pEnableBotsCheck->IsSelected());
	SetControlEnabled("SkillLevel2", m_pEnableBotsCheck->IsSelected());
	SetControlEnabled("SkillLevel3", m_pEnableBotsCheck->IsSelected());
	SetControlEnabled("BotQuotaCombo", m_pEnableBotsCheck->IsSelected());
	SetControlEnabled("BotQuotaLabel", m_pEnableBotsCheck->IsSelected());
	SetControlEnabled("BotDifficultyLabel", m_pEnableBotsCheck->IsSelected());
}
