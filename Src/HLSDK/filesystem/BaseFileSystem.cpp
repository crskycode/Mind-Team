//========= Copyright ?1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "BaseFileSystem.h"
#include "characterset.h"
#include "filesystem_helpers.h"
#include "tier0/dbg.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define BSPOUTPUT	0	// bsp output flag -- determines type of fs_log output to generate


CBaseFileSystem* CBaseFileSystem::CSearchPath::m_fs = 0;

static void FixSlashes( char *str );
static void FixPath( char *str );
static void StripFilename (char *path);
static char s_pScratchFileName[ MAX_PATH ];

//-----------------------------------------------------------------------------
// constructor
//-----------------------------------------------------------------------------

CBaseFileSystem::CBaseFileSystem()
//: m_OpenedFiles( 0, 32, OpenedFileLessFunc )
{
	CSearchPath::m_fs = this;

	m_fwLevel		= FILESYSTEM_WARNING_REPORTUNCLOSED;
	m_pfnWarning	= NULL;
	m_nOpenCount	= 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseFileSystem::~CBaseFileSystem()
{
	RemoveAllSearchPaths();
	Trace_DumpUnclosedFiles();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseFileSystem::Mount()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseFileSystem::Unmount()
{
}


//-----------------------------------------------------------------------------
// Computes a full write path
//-----------------------------------------------------------------------------
inline void CBaseFileSystem::ComputeFullWritePath( char* pDest, const char *pRelativePath, const char *pWritePathID )
{
	strcpy( pDest, GetWritePath(pWritePathID) );
	strcat( pDest, pRelativePath );
	FixSlashes( pDest );
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : src1 - 
//			src2 - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseFileSystem::OpenedFileLessFunc( COpenedFile const& src1, COpenedFile const& src2 )
{
	return src1.m_pFile < src2.m_pFile;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *filename - 
//			*options - 
// Output : FILE
//-----------------------------------------------------------------------------
FILE *CBaseFileSystem::Trace_FOpen( const char *filename, const char *options, bool bFromCache )
{
	FILE *fp = FS_fopen( filename, options, bFromCache );
	if ( fp )
	{
		COpenedFile		file;

		file.SetName( filename );
		file.m_pFile	= fp;

		//m_OpenedFiles.Insert( file );
		m_OpenedFiles.AddToTail( file );
		m_nOpenCount++;

		if ( m_fwLevel >= FILESYSTEM_WARNING_REPORTALLACCESSES )
		{
			Warning( FILESYSTEM_WARNING_REPORTALLACCESSES, "---FS:  open %s %p %i\n", filename, fp, m_nOpenCount );
		}
	}

	return fp;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *fp - 
//-----------------------------------------------------------------------------
void CBaseFileSystem::Trace_FClose( FILE *fp )
{
	if ( fp )
	{
		COpenedFile file;
		file.m_pFile = fp;

		int result = m_OpenedFiles.Find( file );
		if ( result != -1 /*m_OpenedFiles.InvalidIdx()*/ )
		{
			COpenedFile found = m_OpenedFiles[ result ];
			m_nOpenCount--;
			if ( m_fwLevel >= FILESYSTEM_WARNING_REPORTALLACCESSES )
			{
				Warning( FILESYSTEM_WARNING_REPORTALLACCESSES, "---FS:  close %s %p %i\n", found.GetName(), fp, m_nOpenCount );
			}
			m_OpenedFiles.Remove( result );
		}
		else
		{
			Assert( 0 );

			if ( m_fwLevel >= FILESYSTEM_WARNING_REPORTALLACCESSES )
			{
				Warning( FILESYSTEM_WARNING_REPORTALLACCESSES, "Tried to close unknown file pointer %p\n", fp );
			}
		}

		FS_fclose( fp );
	}
}


void CBaseFileSystem::Trace_FRead( int size, FILE* fp )
{
}

void CBaseFileSystem::Trace_FWrite( int size, FILE* fp )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseFileSystem::Trace_DumpUnclosedFiles( void )
{
	for ( int i = 0 ; i < m_OpenedFiles.Count(); i++ )
	{
		COpenedFile *found = &m_OpenedFiles[ i ];

		if ( m_fwLevel >= FILESYSTEM_WARNING_REPORTUNCLOSED )
		{
			Warning( FILESYSTEM_WARNING_REPORTUNCLOSED, "File %s was never closed\n", found->GetName() );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseFileSystem::PrintOpenedFiles( void )
{
	Trace_DumpUnclosedFiles();
}

//-----------------------------------------------------------------------------
// Search path
//-----------------------------------------------------------------------------

struct packfile_t
{
	char name[56];
	int filepos;
	int filelen;
};

struct packheader_t
{
	char id[4];
	int dirofs;
	int dirlen;
};

struct packfile64_t
{
	char name[112];
	int64 filepos;
	int64 filelen;
};

struct packheader64_t
{
	char id[4];
	int64 dirofs;
	int64 dirlen;
};

struct packappenededheader_t
{
	char id[8];
	int64 packheaderpos;
	int64 originalfilesize;
};

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : packfile - 
//			offsetofpackinmetafile - if the .pak is embedded in another file, then all of it's offsets
//				need this offset added to them
//-----------------------------------------------------------------------------
bool CBaseFileSystem::PreparePackFile( CSearchPath* packfile, int64 offsetofpackinmetafile )
{
	packheader_t			header;
	int						i;
	packfile_t              *newfiles;
	int						numpackfiles;

	Assert( packfile->m_bIsPackFile );
	Assert( packfile->m_hPackFile );

	FS_fseek( packfile->m_hPackFile->m_pFile, offsetofpackinmetafile + packfile->m_hPackFile->m_nStartOffset, SEEK_SET);
	FS_fread( &header, 1, sizeof( packheader_t ), packfile->m_hPackFile->m_pFile );

	if ( header.id[0] == 'P' && header.id[1] == 'K' && header.id[2] == '6' && header.id[3] == '4' )
		return Prepare64BitPackFile( packfile, offsetofpackinmetafile );

	if ( header.id[0] != 'P' || header.id[1] != 'A' || header.id[2] != 'C' || header.id[3] != 'K' )
	{
		Warning( (FileWarningLevel_t)-1, "%s is not a packfile", packfile->GetPathString() );
		return false;
	}
	
	// Make sure there are some files to parse
	numpackfiles = header.dirlen / sizeof( packfile_t );
	
	if ( numpackfiles > MAX_FILES_IN_PACK )
	{
		Warning( (FileWarningLevel_t)-1, "%s has %i files", packfile->GetPathString(), numpackfiles );
		return false;
	}

	if (  numpackfiles <= 0 )
	{
		// No files, sigh...
		return false;
	}

	newfiles = new packfile_t[ numpackfiles ];
	Assert( newfiles );
	if ( !newfiles )
	{
		Warning( (FileWarningLevel_t)-1, "%s out of memory allocating directoryf for %i files", packfile, numpackfiles );
		return false;
	}

	FS_fseek( packfile->m_hPackFile->m_pFile, offsetofpackinmetafile + packfile->m_hPackFile->m_nStartOffset + header.dirofs, FILESYSTEM_SEEK_HEAD );
	FS_fread( newfiles, 1, header.dirlen, packfile->m_hPackFile->m_pFile );

	// Insert into rb tree
	for ( i=0 ; i<numpackfiles ; i++ )
	{
		CPackFileEntry	lookup;
#ifdef _WIN32
		strlwr( newfiles[i].name );
#endif
		lookup.m_Name		= newfiles[ i ].name;
		lookup.m_nPosition	= newfiles[i].filepos + offsetofpackinmetafile;
		lookup.m_nLength	= newfiles[i].filelen;

		packfile->m_PackFiles.Insert( lookup );
	}
	
	packfile->m_nNumPackFiles = numpackfiles;

	delete[] newfiles;

	return true;
}

bool CBaseFileSystem::Prepare64BitPackFile( CSearchPath* packfile, int64 offsetofpackinmetafile )
{
	packheader64_t			header;
	int						i;
	packfile64_t              *newfiles;
	int						numpackfiles;

	Assert( packfile->m_bIsPackFile );
	Assert( packfile->m_hPackFile );

	FS_fseek( packfile->m_hPackFile->m_pFile, offsetofpackinmetafile + packfile->m_hPackFile->m_nStartOffset, SEEK_SET);
	FS_fread( &header, 1, sizeof( packheader64_t ), packfile->m_hPackFile->m_pFile );

	if ( header.id[0] != 'P' || header.id[1] != 'K' || header.id[2] != '6' || header.id[3] != '4' )
	{
		Warning( (FileWarningLevel_t)-1, "%s is not a packfile", packfile->GetPathString() );
		return false;
	}
	
	// Make sure there are some files to parse
	numpackfiles = header.dirlen / sizeof( packfile64_t );
	
	if ( numpackfiles > MAX_FILES_IN_PACK )
	{
		Warning( (FileWarningLevel_t)-1, "%s has %i files", packfile->GetPathString(), numpackfiles );
		return false;
	}

	if (  numpackfiles <= 0 )
	{
		// No files, sigh...
		return false;
	}

	newfiles = new packfile64_t[ numpackfiles ];
	Assert( newfiles );
	if ( !newfiles )
	{
		Warning( (FileWarningLevel_t)-1, "%s out of memory allocating directoryf for %i files", packfile, numpackfiles );
		return false;
	}

	FS_fseek( packfile->m_hPackFile->m_pFile, offsetofpackinmetafile + packfile->m_hPackFile->m_nStartOffset + header.dirofs, FILESYSTEM_SEEK_HEAD );
	FS_fread( newfiles, 1, header.dirlen, packfile->m_hPackFile->m_pFile );

	// Insert into rb tree
	for ( i=0 ; i<numpackfiles ; i++ )
	{
		CPackFileEntry	lookup;
#ifdef _WIN32
		strlwr( newfiles[i].name );
#endif
		lookup.m_Name		= newfiles[i].name;
		lookup.m_nPosition	= newfiles[i].filepos  + offsetofpackinmetafile;
		lookup.m_nLength	= newfiles[i].filelen;

		packfile->m_PackFiles.Insert( lookup );
	}
	
	packfile->m_nNumPackFiles = numpackfiles;

	delete[] newfiles;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Search pPath for pak?.pak files and add to search path if found
// Input  : *pPath - 
//-----------------------------------------------------------------------------
void CBaseFileSystem::AddPackFiles( const char *pPath )
{
	int pakcount = 0;
	int i;
	for (i=0 ; ; i++)
	{
		char		pakfile[ 512 ];
		char		fullpath[ 512 ];

		sprintf( pakfile, "pak%i.pak", i );
		sprintf( fullpath, "%s%s", pPath, pakfile );

		FixSlashes( fullpath );

		struct	_stat buf;
		if( FS_stat( fullpath, &buf ) == -1 )
			break;

		++pakcount;
	}

	// Add any pak files in the format pak0.pak pak1.pak . . . 
	// Add them backwards so pak1 is higher priority than pak0, etc.
	char pakfile[ 512 ];
	for ( i=pakcount-1 ; i >= 0; i-- )
	{
		sprintf( pakfile, "pak%i.pak", i );

		AddPackFileFromPath( pPath, pakfile, false, "" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Wipe all map (.bsp) pak file search paths
//-----------------------------------------------------------------------------
void CBaseFileSystem::RemoveAllMapSearchPaths( void )
{
	for( int i = m_SearchPaths.Count() - 1; i >= 0; i-- )
	{
		if( !m_SearchPaths[i].m_bIsMapPath )
			continue;
		
		m_SearchPaths.Remove( i );
	}
}



//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPath - 
//-----------------------------------------------------------------------------
void CBaseFileSystem::AddMapPackFile( const char *pPath )
{
}

//-----------------------------------------------------------------------------
// Purpose: Adds the specified pack file to the list
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseFileSystem::AddPackFile( const char *pFileName, const char *pathID )
{
	return AddPackFileFromPath( "", pFileName, true, pathID );
}

//-----------------------------------------------------------------------------
// Purpose: Adds a pack file from the specified path
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseFileSystem::AddPackFileFromPath( const char *pPath, const char *pakfile, bool bCheckForAppendedPack, const char *pathID )
{
	char fullpath[ MAX_PATH ];
	_snprintf( fullpath, sizeof(fullpath), "%s%s", pPath, pakfile );
	FixSlashes( fullpath );

	struct	_stat buf;
	if ( FS_stat( fullpath, &buf ) == -1 )
		return false;

	// Add this pack file to the search path:
	int nIndex = m_SearchPaths.AddToTail();
	CSearchPath *sp = &m_SearchPaths[ nIndex ];
	sp->m_Path = pPath;
	sp->m_PathID = pathID;
	sp->m_bIsPackFile = true;
	sp->m_bIsRezFile = false;
	sp->m_lPackFileTime = GetFileTime( pakfile );
	sp->m_hPackFile = new CFileHandle;
	sp->m_hPackFile->m_pFile = Trace_FOpen( fullpath, "rb", false );

	int headeroffset = 0;

	if ( bCheckForAppendedPack )
	{
		packappenededheader_t appended;

		FS_fseek( sp->m_hPackFile->m_pFile, -sizeof( packappenededheader_t ), FILESYSTEM_SEEK_TAIL );
		FS_fread( &appended, 1, sizeof( packappenededheader_t ), sp->m_hPackFile->m_pFile );

		if ( appended.id[0] != 'P' || appended.id[1] != 'A' || appended.id[2] != 'C' || appended.id[3] != 'K'
			|| appended.id[4] != 'A' || appended.id[5] != 'P' || appended.id[6] != 'P' || appended.id[7] != 'E' )
		{
			// Failed for some reason, ignore it
			Trace_FClose( sp->m_hPackFile->m_pFile );
			m_SearchPaths.Remove( nIndex );

			return false;
		}

		headeroffset = appended.packheaderpos;
	}

	if ( !PreparePackFile( sp, headeroffset ) )
	{
		// Failed for some reason, ignore it
		Trace_FClose( sp->m_hPackFile->m_pFile );
		m_SearchPaths.Remove( nIndex );

		return false;
	}

	m_PackFileHandles.AddToTail( sp->m_hPackFile->m_pFile );
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Search pPath for rf?.rez files and add to search path if found
// Input  : *pPath - 
//-----------------------------------------------------------------------------
void CBaseFileSystem::AddRezFiles( const char *pPath )
{
	int pakcount = 0;
	int i;
	for (i=1 ; ; i++)
	{
		char		pakfile[ 512 ];
		char		fullpath[ 512 ];

		sprintf( pakfile, "rf%03i.rez", i );
		sprintf( fullpath, "%s%s", pPath, pakfile );

		FixSlashes( fullpath );

		struct	_stat buf;
		if( FS_stat( fullpath, &buf ) == -1 )
			break;

		++pakcount;
	}

	// Add any rez files in the format rf001.rez rf002.rez . . . 
	char pakfile[ 512 ];
	for ( i=1 ; i < pakcount+1; i++ )
	{
		sprintf( pakfile, "rf%03i.rez", i );

		AddRezFileFromPath( pPath, pakfile, "" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Adds a rez file from the specified path
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseFileSystem::AddRezFileFromPath( const char *pPath, const char *rezfile, const char *pathID )
{
	char fullpath[ MAX_PATH ];
	_snprintf( fullpath, sizeof(fullpath), "%s%s", pPath, rezfile );
	FixSlashes( fullpath );

	struct	_stat buf;
	if ( FS_stat( fullpath, &buf ) == -1 )
		return false;

	// Add this rez file to the search path:
	int nIndex = m_SearchPaths.AddToTail();
	CSearchPath *sp = &m_SearchPaths[ nIndex ];
	sp->m_Path = pPath;
	sp->m_PathID = pathID;
	sp->m_bIsPackFile = false;
	sp->m_bIsRezFile = true;
	sp->m_pRezMgr = new CRezMgr;
	sp->m_bAllowWrite = false;

	if ( !sp->m_pRezMgr->Open( fullpath, TRUE, FALSE ) )
	{
		delete sp->m_pRezMgr;
		m_SearchPaths.Remove( nIndex );
		return false;
	}

	sp->m_pRezMgr->SetDirSeparators( "/\\" );

	return true;
}

BOOL WildcardMatch(char *lpFilename, char *lpWildcard);

char *CBaseFileSystem::SearchRezFile( FileFindHandle_t *pHandle, FindData_t *pFindData, bool first )
{
	static char sPath[MAX_PATH];

	char sName[MAX_PATH];
	char sType[5];

	if ( first )
	{
		pFindData->m_pRezType = NULL;
		pFindData->m_pRezItem = NULL;

		for (CRezTyp *pType = pFindData->m_pRezDir->GetFirstType(); pType; pType = pFindData->m_pRezDir->GetNextType( pType ) )
		{
			pFindData->m_pRezMgr->TypeToStr( pType->GetType(), sType );
			strlwr( sType );

			if ( WildcardMatch( sType, pFindData->m_pRezMatchExt.Base() ) )
			{
				pFindData->m_pRezType = pType;
				break;
			}
		}

		if ( !pFindData->m_pRezType )
			return NULL;

		pFindData->m_pRezItem = pFindData->m_pRezDir->GetFirstItem( pFindData->m_pRezType );

		if ( !pFindData->m_pRezItem )
			return NULL;

		pFindData->m_pRezMgr->TypeToStr( pFindData->m_pRezType->GetType(), sType );

		if ( sType[0] )
			sprintf( sPath, "%s.%s", pFindData->m_pRezItem->GetName(), sType );
		else
			sprintf( sPath, "%s", pFindData->m_pRezItem->GetName() );

		//
		*pHandle = m_FindData.Count() - 1;

		return sPath;
	}

NEXTTYPE:
	if ( !pFindData->m_pRezType )
		return NULL;

	if ( !pFindData->m_pRezItem )
		pFindData->m_pRezItem = pFindData->m_pRezDir->GetFirstItem( pFindData->m_pRezType );
	else
		pFindData->m_pRezItem = pFindData->m_pRezDir->GetNextItem( pFindData->m_pRezItem );

	for ( ; 
		pFindData->m_pRezItem; 
		pFindData->m_pRezItem = pFindData->m_pRezDir->GetNextItem( pFindData->m_pRezItem ) )
	{
		strcpy( sName, pFindData->m_pRezItem->GetName() );
		strlwr( sName );

		if ( WildcardMatch( sName, pFindData->m_pRezMatchName.Base() ) )
		{
			pFindData->m_pRezMgr->TypeToStr( pFindData->m_pRezType->GetType(), sType );

			if ( sType[0] )
				sprintf( sPath, "%s.%s", pFindData->m_pRezItem->GetName(), sType );
			else
				sprintf( sPath, "%s", pFindData->m_pRezItem->GetName() );

			return sPath;
		}
	}

	for ( pFindData->m_pRezType = pFindData->m_pRezDir->GetNextType( pFindData->m_pRezType ); 
		pFindData->m_pRezType; 
		pFindData->m_pRezType = pFindData->m_pRezDir->GetNextType( pFindData->m_pRezType ) )
	{
		pFindData->m_pRezMgr->TypeToStr( pFindData->m_pRezType->GetType(), sType );
		strlwr( sType );

		if ( WildcardMatch( sType, pFindData->m_pRezMatchExt.Base() ) )
		{
			goto NEXTTYPE;
		}
	}

	pFindData->m_pRezType = NULL;
	pFindData->m_pRezItem = NULL;

	return NULL;
}

void CBaseFileSystem::PrintSearchPaths( void )
{
	int i;
	Warning( (FileWarningLevel_t)-1, "---------------\n" );
	Warning( (FileWarningLevel_t)-1, "Paths:\n" );
	for( i = 0; i < m_SearchPaths.Count(); i++ )
	{
		CSearchPath *pSearchPath = &m_SearchPaths[i];
		Warning( (FileWarningLevel_t)-1, "\"%s\" \"%s\"\n", ( const char * )pSearchPath->GetPathString(), ( const char * )pSearchPath->GetPathIDString() );
	}
}


//-----------------------------------------------------------------------------
// Create the search path.
//-----------------------------------------------------------------------------
void CBaseFileSystem::AddSearchPathNoWrite( const char *pPath, const char *pathID )
{
	AddSearchPathInternal( pPath, pathID, false );
}

//-----------------------------------------------------------------------------
// Create the search path.
//-----------------------------------------------------------------------------
void CBaseFileSystem::AddSearchPath( const char *pPath, const char *pathID )
{
	AddSearchPathInternal( pPath, pathID, true );
}

//-----------------------------------------------------------------------------
// Purpose: This is where search paths are created.  map files are created at head of list (they occur after
//  file system paths have already been set ) so they get highest priority.  Otherwise, we add the disk (non-packfile)
//  path and then the paks if they exist for the path
// Input  : *pPath - 
//-----------------------------------------------------------------------------
void CBaseFileSystem::AddSearchPathInternal( const char *pPath, const char *pathID, bool bAllowWrite )
{
	// Map pak files have their own handler
	if ( strstr( pPath, ".bsp" ) )
	{
		AddMapPackFile( pPath );
		return;
	}

	// Clean up the name
	char *newPath;
	// +2 for '\0' and potential slash added at end.
	newPath = ( char * )_alloca( MAX_PATH + 2 );

	// Is it an absolute path?
	if ( strchr( pPath, ':' ) )
	{
		strcpy( newPath, pPath );
	}
	else
	{
		GetCurrentDirectory( newPath, MAX_PATH );
		FixPath( newPath );

		if ( !strcmp( pPath, "." ) )
			strcat( newPath, pPath );
	}

#ifdef _WIN32
	strlwr( newPath );
#endif
	FixPath( newPath );

	// Make sure that it doesn't already exist
	CUtlSymbol pathSym, pathIDSym;
	pathSym = newPath;
	pathIDSym = pathID;
	int i;
	for( i = 0; i < m_SearchPaths.Count(); i++ )
	{
		CSearchPath *pSearchPath = &m_SearchPaths[i];
		if( pSearchPath->m_Path == pathSym && pSearchPath->m_PathID == pathIDSym )
		{
			return;
		}
	}
	
	// Add to tail of list
	int newIndex = m_SearchPaths.AddToTail();

	// Grab last entry and set the path
	CSearchPath *sp = &m_SearchPaths[ newIndex ];
	
	sp->m_Path = pathSym;
	sp->m_PathID = pathIDSym;
	sp->m_bAllowWrite = bAllowWrite;

	// Add pack files for this path next
	AddPackFiles( newPath );
	// Add rez files for this path next
	AddRezFiles( newPath );

#ifdef _DEBUG
	PrintSearchPaths();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPath - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseFileSystem::RemoveSearchPath( const char *pPath )
{
	char *newPath = NULL;

	// +2 for '\0' and potential slash added at end.
	newPath = ( char * )_alloca( strlen( pPath ) + 2 );

	// Is it an absolute path?
	if ( strchr( pPath, ':' ) )
	{
		strcpy( newPath, pPath );
	}
	else
	{
		GetCurrentDirectory( newPath, MAX_PATH );
		FixPath( newPath );

		if ( !strcmp( pPath, "." ) )
			strcat( newPath, pPath );
	}

#ifdef _WIN32
	strlwr( newPath );
#endif
	FixPath( newPath );

	CUtlSymbol lookup = newPath;

	bool bret = false;

	// Count backward since we're possibly deleting one or more pack files, too
	int i;
	for( i = m_SearchPaths.Count() - 1; i >= 0; i-- )
	{
		if( newPath && m_SearchPaths[i].m_Path != lookup )
			continue;
		
		m_SearchPaths.Remove( i );
		bret = true;
	}
	return bret;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBaseFileSystem::CSearchPath *CBaseFileSystem::FindWritePath( const char *pathID )
{
	CUtlSymbol lookup = pathID;

	// a pathID has been specified, find the first match in the path list
	for (int i = 0; i < m_SearchPaths.Count(); i++)
	{
		if (!m_SearchPaths[i].m_bAllowWrite)
			continue;

		if ( !pathID || (m_SearchPaths[i].m_PathID == lookup) )
		{
			return &m_SearchPaths[i];
		}
	}

	return NULL;
}


//-----------------------------------------------------------------------------
// Purpose: Finds a search path that should be used for writing to, given a pathID
//-----------------------------------------------------------------------------
const char *CBaseFileSystem::GetWritePath( const char *pathID )
{
	CSearchPath *pSearchPath = NULL;
	if (pathID)
	{
		pSearchPath = FindWritePath( pathID );
		if (pSearchPath)
		{
			return pSearchPath->GetPathString();
		}
	}

	// Choose the topmost writeable path in the list to write to 
	// FIXME: Is this really what we want to be doing here?
	pSearchPath = FindWritePath( NULL ); // Should we find "DEFAULT_WRITE_PATH" ?
	if (pSearchPath)
	{
		return pSearchPath->GetPathString();
	}

	// Hope this is reasonable!!
	return ".\\";
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseFileSystem::RemoveAllSearchPaths( void )
{
	m_SearchPaths.Purge();
	m_PackFileHandles.Purge();
}


//-----------------------------------------------------------------------------
// Purpose: The base file search goes through here
// Input  : *path - 
//			*pFileName - 
//			*pOptions - 
//			packfile - 
//			*filetime - 
// Output : FileHandle_t
//-----------------------------------------------------------------------------
FileHandle_t CBaseFileSystem::FindFile( const CSearchPath *path, const char *pFileName, const char *pOptions, bool bFromCache )
{
	CFileHandle *fh;

	if ( path->m_bIsPackFile )
	{
		// Search the tree for the filename
		CPackFileEntry search;
		char *temp = (char *)_alloca( strlen( pFileName ) + 1 );
		strcpy( temp, pFileName );
		strlwr( temp );

		search.m_Name = temp;

		int searchresult = path->m_PackFiles.Find( search );

		if ( searchresult != path->m_PackFiles.InvalidIndex() )
		{
			CPackFileEntry result = path->m_PackFiles[ searchresult ];

			FS_fseek( path->m_hPackFile->m_pFile, result.m_nPosition + path->m_hPackFile->m_nStartOffset, SEEK_SET );

			fh = new CFileHandle;

			fh->m_pFile = path->m_hPackFile->m_pFile;
			fh->m_nStartOffset = result.m_nPosition;
			fh->m_nLength = result.m_nLength;
			fh->m_nFileTime = path->m_lPackFileTime;
			fh->m_bPack = true;
			fh->m_bRez = false;

			return (FileHandle_t)fh;
		}
	}
	else if ( path->m_bIsRezFile )
	{
		if ( path->m_pRezMgr )
		{
			CRezItm *pItem = path->m_pRezMgr->GetRezFromDosPath( pFileName );

			if ( pItem )
			{
				fh = new CFileHandle;

				fh->m_pFile = NULL;
				fh->m_nStartOffset = 0;
				fh->m_nLength = pItem->GetSize();
				fh->m_nFileTime = 0;
				fh->m_bPack = false;
				fh->m_bRez = true;
				fh->m_pRezItem = pItem;
				fh->m_nRezPos = 0;

				return (FileHandle_t)fh;
			}
		}

		return (FileHandle_t)NULL;
	}
	else
	{
		bool bWrite = ( !strstr( pOptions, "w" ) && !strstr( pOptions, "+" ) || path->m_bAllowWrite );

		if ( bWrite )
		{
			char *pTmpFileName = ( char * )_alloca( strlen( path->GetPathString() ) + strlen( pFileName ) + 1 );
			strcpy( pTmpFileName, path->GetPathString() );
			strcat( pTmpFileName, pFileName );
			FixSlashes( pTmpFileName );

			FILE *fp = Trace_FOpen( pTmpFileName, pOptions, bFromCache );
			if( fp )
			{
				CFileHandle *fh = new CFileHandle;

				fh->m_pFile = fp;
				fh->m_bPack = false;
				fh->m_bRez = false;
				struct	_stat buf;
				int sr = FS_stat( pTmpFileName, &buf );
				if ( sr == -1 )
				{
					Warning( (FileWarningLevel_t)-1, "_stat on file %s which appeared to exist failed!!!\n",
						pTmpFileName );
				}
				fh->m_nFileTime = max(buf.st_mtime, buf.st_ctime);
				fh->m_nLength = buf.st_size;
				fh->m_nStartOffset = 0;

				return ( FileHandle_t )fh;
			}
		}
	}

	return (FileHandle_t)NULL;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
FileHandle_t CBaseFileSystem::OpenForRead( const char *pFileName, const char *pOptions, const char *pathID )
{
	CUtlSymbol lookup;
	if (pathID)
	{
		lookup = pathID;
	}

	// Opening for READ needs to search search paths
	int i;
	for( i = 0; i < m_SearchPaths.Count(); i++ )
	{
		if (pathID && m_SearchPaths[i].m_PathID != lookup)
			continue;

		FileHandle_t filehandle = FindFile( &m_SearchPaths[ i ], pFileName, pOptions, false );
		if ( filehandle == 0 )
			continue;

		return filehandle;
	}

	return ( FileHandle_t )0;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
FileHandle_t CBaseFileSystem::OpenForWrite( const char *pFileName, const char *pOptions, const char *pathID )
{
	// Opening for write or append uses the write path
	// Unless an absolute path is specified...
	const char *pTmpFileName;
	if ( strstr( pFileName, ":" ) || pFileName[0] == '/' || pFileName[0] == '\\' )
	{
		pTmpFileName = pFileName;
	}
	else
	{
		ComputeFullWritePath( s_pScratchFileName, pFileName, pathID );
		pTmpFileName = s_pScratchFileName; 
	}

	FILE *fp = Trace_FOpen( pTmpFileName, pOptions, false );
	if( !fp )
		return ( FileHandle_t )0;

	CFileHandle *fh = new CFileHandle;

	struct	_stat buf;
	int sr = FS_stat( pTmpFileName, &buf );
	if ( sr == -1 )
	{
		Warning( (FileWarningLevel_t)-1, "_stat on file %s which appeared to exist failed!!!\n",
			pTmpFileName );
	}
	fh->m_nFileTime = buf.st_mtime;
	fh->m_nLength = buf.st_size;
	fh->m_nStartOffset = 0;

	fh->m_bPack = false;
	fh->m_bRez = false;
	fh->m_pFile = fp;

	return ( FileHandle_t )fh;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
FileHandle_t CBaseFileSystem::OpenFromCacheForRead( const char *pFileName, const char *pOptions, const char *pathID )
{
	CUtlSymbol lookup;
	if (pathID)
	{
		lookup = pathID;
	}

	// Opening for READ needs to search search paths
	int i;
	for( i = 0; i < m_SearchPaths.Count(); i++ )
	{
		if (pathID && m_SearchPaths[i].m_PathID != lookup)
			continue;

		FileHandle_t filehandle = FindFile( &m_SearchPaths[ i ], pFileName, pOptions, true );
		if ( filehandle == 0 )
			continue;

		return filehandle;
	}

	return ( FileHandle_t )0;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
FileHandle_t CBaseFileSystem::Open( const char *pFileName, const char *pOptions, const char *pathID )
{
	// Try each of the search paths in succession
	// FIXME: call createdirhierarchy upon opening for write.
	if( strstr( pOptions, "r" ) && !strstr( pOptions, "+" ) )
	{
		return OpenForRead( pFileName, pOptions, pathID );
	}

	return OpenForWrite( pFileName, pOptions, pathID );
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseFileSystem::Close( FileHandle_t file )
{
	CFileHandle *fh = ( CFileHandle *)file;
	if ( !fh )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to Close NULL file handle!\n" );
		return;
	}
	
	if ( !fh->m_pFile && !fh->m_bRez )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to Close NULL file pointer inside valid file handle!\n" );
		return;
	}

	// Don't close the underlying fp if this is a pack file file pointer
	bool isPackFilePointer = ( m_PackFileHandles.Find( fh->m_pFile ) != m_PackFileHandles.InvalidIndex() ) ? true : false;
	if ( !isPackFilePointer )
	{
		Trace_FClose( fh->m_pFile );
		fh->m_pFile = 0;
	}

	delete fh;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseFileSystem::Seek( FileHandle_t file, int pos, FileSystemSeek_t whence )
{
	CFileHandle *fh = ( CFileHandle *)file;
	if ( !fh )
	{
		Warning( (FileWarningLevel_t)-1, "Tried to Seek NULL file handle!\n" );
		return;
	}
	
	if ( !fh->m_pFile && !fh->m_bRez )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to Seek NULL file pointer inside valid file handle!\n" );
		return;
	}

	int seekType;
	if (whence == FILESYSTEM_SEEK_HEAD)
		seekType = SEEK_SET;
	else if (whence == FILESYSTEM_SEEK_CURRENT)
		seekType = SEEK_CUR;
	else
		seekType = SEEK_END;

	// Pack files get special handling
	if ( fh->m_bPack )
	{
		if ( whence == FILESYSTEM_SEEK_CURRENT )
		{
			// Just offset from current position
			FS_fseek( fh->m_pFile, pos, seekType );
		}
		else if ( whence == FILESYSTEM_SEEK_HEAD )
		{
			// Go to start and offset by pos
			FS_fseek( fh->m_pFile, pos + fh->m_nStartOffset, seekType );
		}
		else
		{
			// Go to end and offset by pos
			FS_fseek( fh->m_pFile, pos + fh->m_nStartOffset + fh->m_nLength, seekType );
		}
	}
	else if ( fh->m_bRez )
	{
		if ( whence == FILESYSTEM_SEEK_CURRENT )
		{
			fh->m_nRezPos += pos;
		}
		else if ( whence == FILESYSTEM_SEEK_HEAD )
		{
			fh->m_nRezPos = pos;
		}
		else
		{
			fh->m_nRezPos = fh->m_nLength + pos;
		}

		fh->m_pRezItem->Seek( fh->m_nRezPos );
	}
	else
	{
		FS_fseek( fh->m_pFile, pos, seekType );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : file - 
// Output : unsigned int
//-----------------------------------------------------------------------------
unsigned int CBaseFileSystem::Tell( FileHandle_t file )
{
	CFileHandle *fh = ( CFileHandle *)file;
	if ( !fh )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to Tell NULL file handle!\n" );
		return 0;
	}

	if ( !fh->m_pFile && !fh->m_bRez )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to Tell NULL file pointer inside valid file handle!\n" );
		return 0;
	}

	if ( fh->m_bRez )
	{
		return fh->m_nRezPos;
	}

	// Pack files are relative
	return FS_ftell( fh->m_pFile ) - fh->m_nStartOffset;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : file - 
// Output : unsigned int
//-----------------------------------------------------------------------------
unsigned int CBaseFileSystem::Size( FileHandle_t file )
{
	CFileHandle *fh = ( CFileHandle *)file;
	if ( !fh )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to Size NULL file handle!\n" );
		return 0;
	}

	if ( !fh->m_pFile && !fh->m_bRez )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to Size NULL file pointer inside valid file handle!\n" );
		return 0;
	}

	return fh->m_nLength;
}



//-----------------------------------------------------------------------------
// Purpose: 
// Input  : file - 
// Output : unsigned int
//-----------------------------------------------------------------------------
unsigned int CBaseFileSystem::Size( const char* pFileName )
{
	// Handle adding in searth paths
	int i;
	int size = 0;
	for( i = 0; i < m_SearchPaths.Count(); i++ )
	{
		size = FastFindFile( &m_SearchPaths[ i ], pFileName );
		if ( size > 0 )
		{
			break;
		}
	}
	return size;
}

int CBaseFileSystem::FastFindFile( const CSearchPath *path, const char *pFileName )
{
	struct	_stat buf;

	if ( path->m_bIsPackFile )
	{
		// Search the tree for the filename
		CPackFileEntry search;
		char *temp = (char *)_alloca( strlen( pFileName ) + 1 );
		strcpy( temp, pFileName );
		strlwr( temp );

		search.m_Name = temp;

		int searchresult = path->m_PackFiles.Find( search );

		if ( searchresult != path->m_PackFiles.InvalidIndex() )
		{
			CPackFileEntry result = path->m_PackFiles[ searchresult ];
			return (result.m_nLength);
		}
	}
	else if ( path->m_bIsRezFile )
	{
		CRezItm *pItem = path->m_pRezMgr->GetRezFromDosPath( pFileName );

		if ( pItem )
		{
			return pItem->GetSize();
		}
	}
	else
	{
		char *pTmpFileName = ( char * )_alloca( strlen( path->GetPathString() ) + strlen( pFileName ) + 1 );
		strcpy( pTmpFileName, path->GetPathString() );
		strcat( pTmpFileName, pFileName );
		FixSlashes( pTmpFileName );

		if( _stat( pTmpFileName, &buf ) != -1 )
		{
			return buf.st_size;
		}
	}

	return ( -1 );
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CBaseFileSystem::EndOfFile( FileHandle_t file )
{
	CFileHandle *fh = ( CFileHandle *)file;
	if ( !fh )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to EndOfFile NULL file handle!\n" );
		return true;
	}

	if ( !fh->m_pFile && !fh->m_bRez )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to EndOfFile NULL file pointer inside valid file handle!\n" );
		return true;
	}

	if ( fh->m_bPack )
	{
		if ( FS_ftell( fh->m_pFile ) >=
			fh->m_nStartOffset + fh->m_nLength )
		{
			return true;
		}
		return false;
	}
	else if ( fh->m_bRez )
	{
		if ( fh->m_nRezPos >= fh->m_nLength )
		{
			return true;
		}
		return false;
	}
	return !!FS_feof( fh->m_pFile );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CBaseFileSystem::Read( void *pOutput, int size, FileHandle_t file )
{
	CFileHandle *fh = ( CFileHandle *)file;
	if ( !fh )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to Read NULL file handle!\n" );
		return 0;
	}

	if ( !fh->m_pFile && !fh->m_bRez )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to Read NULL file pointer inside valid file handle!\n" );
		return 0;
	}

	if ( fh->m_bRez )
	{
		int sizeRead = fh->m_pRezItem->Read( pOutput, size, fh->m_nRezPos );
		fh->m_nRezPos += sizeRead;
		return sizeRead;
	}

	int result = FS_fread( pOutput, 1, size, fh->m_pFile  );

	return result;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CBaseFileSystem::Write( void const* pInput, int size, FileHandle_t file )
{
	CFileHandle *fh = ( CFileHandle *)file;
	if ( !fh )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to Write NULL file handle!\n" );
		return 0;
	}

	if ( !fh->m_pFile && !fh->m_bRez )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to Write NULL file pointer inside valid file handle!\n" );
		return 0;
	}

	if ( fh->m_bRez )
	{
		// Can not write to rez item
		return 0;
	}

	return FS_fwrite( (void *)pInput, 1, size, fh->m_pFile  );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CBaseFileSystem::FPrintf( FileHandle_t file, char *pFormat, ... )
{
	CFileHandle *fh = ( CFileHandle *)file;
	if ( !fh )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to fprintf NULL file handle!\n" );
		return 0;
	}

	if ( !fh->m_pFile && !fh->m_bRez )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to fprintf NULL file pointer inside valid file handle!\n" );
		return 0;
	}

	if ( fh->m_bRez )
	{
		// Can not write to rez
		return 0;
	}

	va_list args;
	va_start( args, pFormat );
	int len = FS_vfprintf( fh->m_pFile , pFormat, args );
	va_end( args );
	return len;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CBaseFileSystem::IsOk( FileHandle_t file )
{
	CFileHandle *fh = ( CFileHandle *)file;
	if ( !fh )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to IsOk NULL file handle!\n" );
		return false;
	}

	if ( !fh->m_pFile && !fh->m_bRez )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to IsOk NULL file pointer inside valid file handle!\n" );
		return false;
	}

	if ( fh->m_bRez )
	{
		return true;
	}

	return FS_ferror(fh->m_pFile ) == 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseFileSystem::Flush( FileHandle_t file )
{
	CFileHandle *fh = ( CFileHandle *)file;
	if ( !fh )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to Flush NULL file handle!\n" );
		return;
	}

	if ( !fh->m_pFile && !fh->m_bRez )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to Flush NULL file pointer inside valid file handle!\n" );
		return;
	}

	if ( fh->m_bRez )
	{
		return;
	}

	FS_fflush(fh->m_pFile );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
char *CBaseFileSystem::ReadLine( char *pOutput, int maxChars, FileHandle_t file )
{
	CFileHandle *fh = ( CFileHandle *)file;
	if ( !fh )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to ReadLine NULL file handle!\n" );
		return "";
	}

	if ( !fh->m_pFile && !fh->m_bRez )
	{
		Warning( (FileWarningLevel_t)-1, "FS:  Tried to ReadLine NULL file pointer inside valid file handle!\n" );
		return "";
	}

	if ( fh->m_bRez )
	{
		uint8 *pByte = (uint8 *)pOutput;

		if ( fh->m_nRezPos >= fh->m_nLength )
		{
			return NULL;
		}

		for ( int i = 0; i < maxChars - 1; i++ )
		{
			fh->m_nRezPos += fh->m_pRezItem->Read( pByte, 1, fh->m_nRezPos );

			if ( *pByte == '\n' || fh->m_nRezPos >= fh->m_nLength )
			{
				pByte++;
				*pByte = '\0';
				return (char *)pOutput;
			}

			pByte++;
		}

		*pByte = '\0';
		return (char *)pOutput;
	}

	return FS_fgets( pOutput, maxChars, fh->m_pFile  ); // STEAM ???
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void *CBaseFileSystem::GetReadBuffer( FileHandle_t file, int *outBufferSize, bool failIfNotInCache )
{
	/*if ( !outBufferSize )
		return NULL;

	*outBufferSize = Size( file );

	if ( *outBufferSize <= 0 )
		return NULL;

	byte *buffer = ( byte * )malloc( *outBufferSize );

	if ( !buffer )
	{
		*outBufferSize = 0;
		return NULL;
	}

	Read( buffer, *outBufferSize, file );

	return buffer;*/

	*outBufferSize = 0;
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseFileSystem::ReleaseReadBuffer( FileHandle_t file, void *readBuffer )
{
	/*if ( readBuffer )
	{
		free( readBuffer );
	}*/
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pFileName - 
// Output : long
//-----------------------------------------------------------------------------
long CBaseFileSystem::GetFileTime( const char *pFileName )
{
	int i;
	for( i = 0; i < m_SearchPaths.Count(); i++ )
	{
		FileHandle_t filehandle = FindFile( &m_SearchPaths[ i ], pFileName, "rb", false );
		if ( filehandle == 0 )
			continue;

		CFileHandle *fh = (CFileHandle *)filehandle;
		if ( !fh )
			continue;

		long time = fh->m_nFileTime;

		Close( filehandle );

		return time;
	}
	return 0L;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pString - 
//			maxCharsIncludingTerminator - 
//			fileTime - 
//-----------------------------------------------------------------------------
void CBaseFileSystem::FileTimeToString( char *pString, int maxCharsIncludingTerminator, long fileTime )
{
	strncpy( pString, ctime( (time_t*)&fileTime ), maxCharsIncludingTerminator );
	pString[maxCharsIncludingTerminator-1] = '\0';
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pFileName - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseFileSystem::FileExists( const char *pFileName )
{
	int i;
	for( i = 0; i < m_SearchPaths.Count(); i++ )
	{
		int size = FastFindFile( &m_SearchPaths[ i ], pFileName );
		if ( size == -1 )
			continue;

		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pFileName - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseFileSystem::IsDirectory( const char *pFileName )
{
	struct	_stat buf;
	int i;
	for( i = 0; i < m_SearchPaths.Count(); i++ )
	{
		// Pack files can't be directories
		CSearchPath *sp = &m_SearchPaths[ i ];
		if ( sp->m_bIsPackFile || sp->m_bIsRezFile )
			continue;

		char *pTmpFileName;
		pTmpFileName = ( char * )_alloca( strlen( m_SearchPaths[i].GetPathString() ) + strlen( pFileName ) + 1 );
		strcpy( pTmpFileName, m_SearchPaths[i].GetPathString() );
		strcat( pTmpFileName, pFileName );
		FixSlashes( pTmpFileName );
		if( FS_stat( pTmpFileName, &buf ) != -1 )
		{
			if( buf.st_mode & _S_IFDIR )
			{
				return true;
			}
		}
	}
	return false;
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *path - 
//-----------------------------------------------------------------------------
void CBaseFileSystem::CreateDirHierarchy( const char *pRelativePath, const char *pathID )
{	
	ComputeFullWritePath( s_pScratchFileName, pRelativePath, pathID );
	int len = strlen( s_pScratchFileName ) + 1;

	char *s;
	char *end;
	
	end = s_pScratchFileName + len;
	s = s_pScratchFileName;

	while( s < end )
    {
		if( *s == CORRECT_PATH_SEPARATOR )
        {
			*s = '\0';
#ifdef _WIN32
			_mkdir( s_pScratchFileName );
#elif _LINUX
			mkdir( s_pScratchFileName, S_IRWXU |  S_IRGRP |  S_IROTH );// owner has rwx, rest have r
#endif
			*s = CORRECT_PATH_SEPARATOR;
        }
		s++;
    }
#ifdef _WIN32
	_mkdir( s_pScratchFileName );
#elif _LINUX
	mkdir( s_pScratchFileName, S_IRWXU |  S_IRGRP |  S_IROTH );
#endif

}


int WildCardMatch( const char *mask, const char *name )
{
	//printf("Test:%s %s\n",mask,name);

	if(!strcmp(name,".") || !strcmp(name,"..") ) return 0;

	if(!strcmp(mask,"*.*")) return 1;

	while( *mask && *name )
	{
		if(*mask=='*')
		{
			mask++; // move to the next char in the mask
			if(!*mask) // if this is the end of the mask its a match 
			{
				return 1;
			}
			while(*name && toupper(*name)!=toupper(*mask)) 
			{ // while the two don't meet up again
				name++;
			}
			if(!*name) 
			{ // end of the name
				break; 
			}
		}
		else if (*mask!='?')
		{
			if( toupper(*mask) != toupper(*name) )
			{	// mismatched!
				return 0;
			}
			else
			{	
				mask++;
				name++;
				if( !*mask && !*name) 
				{ // if its at the end of the buffer
					return 1;
				}
				
			}

		}
		else /* mask is "?", we don't care*/
		{
			mask++;
			name++;
		}
	}	
		
	return( !*mask && !*name ); // both of the strings are at the end
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pWildCard - 
//			currentSearchPathID -
//			first -
//-----------------------------------------------------------------------------
char *CBaseFileSystem::SearchPakFile( const char *pWildCard, int currentSearchPathID, bool first )
{
	if ( first )
		m_SearchPaths[currentSearchPathID].m_iCurSearchFile = 0;

	for(	; 
			m_SearchPaths[currentSearchPathID].m_iCurSearchFile < m_SearchPaths[currentSearchPathID].m_nNumPackFiles; 
			m_SearchPaths[currentSearchPathID].m_iCurSearchFile++ )
	{
		const char *file = m_SearchPaths[currentSearchPathID].m_PackFiles[m_SearchPaths[currentSearchPathID].m_iCurSearchFile].m_Name.String();

		if ( WildCardMatch( pWildCard, file ) )
		{
			m_SearchPaths[currentSearchPathID].m_iCurSearchFile++;
			return (char *)file;
		}
	}

	return NULL;
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pWildCard - 
//			*pHandle - 
// Output : const char
//-----------------------------------------------------------------------------
const char *CBaseFileSystem::FindFirst( const char *pWildCard, FileFindHandle_t *pHandle, const char *pathID )
{
 	Assert(pWildCard);
 	Assert(pHandle);

	FileFindHandle_t hTmpHandle = m_FindData.Count();
	m_FindData.AddToTail();
	FindData_t *pFindData = &m_FindData[hTmpHandle];
	Assert(pFindData);
	
	pFindData->wildCardString.AddMultipleToTail( strlen( pWildCard ) + 1 );
	strcpy( pFindData->wildCardString.Base(), pWildCard );
	FixSlashes( pFindData->wildCardString.Base() );
	
	CUtlSymbol lookup = pathID;

	for(	pFindData->currentSearchPathID = 0; 
			pFindData->currentSearchPathID < m_SearchPaths.Count(); 
			pFindData->currentSearchPathID++ )
	{
		if ( !pathID || m_SearchPaths[pFindData->currentSearchPathID].m_PathID == lookup )
		{
			pFindData->limitedPathID = pathID ? (UtlSymId_t)lookup : -1;

			char *fileName = FindFirstHelper( pWildCard, pHandle, pFindData->currentSearchPathID, pFindData );

			if ( fileName )
				return fileName;
		}
	}
	
	// Handle failure here
	pFindData = 0;
	m_FindData.Remove(hTmpHandle);
	*pHandle = -1;

	return NULL;
}

char *CBaseFileSystem::FindFirstHelper( const char *pWildCard, FileFindHandle_t *pHandle, int searchPath, FindData_t *pFindData )
{
	const char *fname = NULL;

	pFindData->m_bIsRezFile = false;

	if ( m_SearchPaths[searchPath].m_bIsPackFile )
	{
		fname = SearchPakFile( pFindData->wildCardString.Base(), searchPath, true );

		if ( fname )
		{
			pFindData->findHandle = INVALID_HANDLE_VALUE;
			*pHandle = m_FindData.Size() - 1;
		}
	}
	else if ( m_SearchPaths[searchPath].m_bIsRezFile )
	{
		pFindData->m_bIsRezFile = true;
		pFindData->m_pRezMgr = m_SearchPaths[searchPath].m_pRezMgr;
		pFindData->m_pRezDir = NULL;
		pFindData->m_pRezType = NULL;
		pFindData->m_pRezItem = NULL;

		char sPath[MAX_PATH];
		char sWild[MAX_PATH];
		char sName[MAX_PATH];
		char sExts[MAX_PATH];

		strcpy( sPath, pWildCard );
		FixSlashes( sPath );
		PathRemoveFileSpec( sPath );

		strcpy( sWild, pWildCard );
		FixSlashes( sWild );
		PathStripPath( sWild );

		strcpy( sName, sWild );
		PathRemoveExtension( sName );
		strlwr( sName );

		char *pExt = PathFindExtension( sWild );
		if ( *pExt == '.' ) pExt++;
		strcpy( sExts, pExt );
		strlwr( sExts );

		// Invalid Wildcard
		if ( !sName[0] || !sExts[0] )
			return NULL;

		pFindData->m_pRezMatchName.RemoveAll();
		pFindData->m_pRezMatchName.AddMultipleToTail( strlen( sName ) + 1 );
		strcpy( pFindData->m_pRezMatchName.Base(), sName );

		pFindData->m_pRezMatchExt.RemoveAll();
		pFindData->m_pRezMatchExt.AddMultipleToTail( strlen( sExts ) + 1 );
		strcpy( pFindData->m_pRezMatchExt.Base(), sExts );

		pFindData->m_pRezDir = pFindData->m_pRezMgr->GetRootDir();

		if ( sPath[0] )
			pFindData->m_pRezDir = pFindData->m_pRezMgr->GetDirFromPath( sPath );

		if ( !pFindData->m_pRezDir )
			return NULL;

		return (char *)SearchRezFile( pHandle, pFindData, true );
	}
	else
	{
		char *pTmpFileName;
		int size;
		size = strlen( m_SearchPaths[searchPath].GetPathString() ) + pFindData->wildCardString.Count();
		pTmpFileName = ( char * )malloc( size + 1 ); // don't need two NULL terminators.
		strcpy( pTmpFileName, m_SearchPaths[pFindData->currentSearchPathID].GetPathString() );
		strcat( pTmpFileName, pFindData->wildCardString.Base() );
		FixSlashes( pTmpFileName );

		pFindData->findHandle = FS_FindFirstFile( pTmpFileName, &pFindData->findData );
		free( pTmpFileName );
		
		if( pFindData->findHandle != INVALID_HANDLE_VALUE )
		{
			*pHandle = m_FindData.Size() - 1;
			return pFindData->findData.cFileName;
		}
	}

	return (char *)fname;
}

// assumes that pSearchWildcard has the proper slash convention.
bool CBaseFileSystem::FileInSearchPaths( const char *pSearchWildcard, 
										   const char *pFileName, int minSearchPathID,
										   int maxSearchPathID )
{
	if( minSearchPathID > maxSearchPathID )
	{
		return false;
	}
	
	// FIGURE OUT THE FILENAME WITHOUT THE SEARCH PATH.
	const char *tmp = &pSearchWildcard[strlen( pSearchWildcard ) - 1];
	while( *tmp != CORRECT_PATH_SEPARATOR && tmp > pSearchWildcard )
	{
		tmp--;
	}
	tmp++;
	if( tmp <= pSearchWildcard )
	{
		Assert( 0 );
		return false;
	}
	int pathStrLen = tmp - pSearchWildcard;
	int fileNameStrLen = strlen( pFileName );
	char *pFileNameWithPath = ( char * )_alloca( pathStrLen + fileNameStrLen + 1 );
	memcpy( pFileNameWithPath, pSearchWildcard, pathStrLen );
	pFileNameWithPath[pathStrLen] = '\0';
	strcat( pFileNameWithPath, pFileName );

	// Check each of the search paths and see if the file exists.
	int i;
	for( i = minSearchPathID; i <= maxSearchPathID; i++ )
	{
		// FIXME: Should this work with PAK files?
		if ( m_SearchPaths[i].m_bIsPackFile )
		{
			int curSearch = m_SearchPaths[i].m_iCurSearchFile;
			bool ret = SearchPakFile( pSearchWildcard, minSearchPathID, true ) != NULL;

			m_SearchPaths[i].m_iCurSearchFile = curSearch;
			if ( ret )
				return ret;

			continue;
		}
		else if ( m_SearchPaths[i].m_bIsRezFile )
		{
			return false;
		}

		char *fullFilePath;
		int len = strlen( m_SearchPaths[i].GetPathString() ) + strlen( pFileNameWithPath );
		fullFilePath = ( char * )_alloca( len + 1 );
		strcpy( fullFilePath, m_SearchPaths[i].GetPathString() );
		strcat( fullFilePath, pFileNameWithPath );
		struct	_stat buf;
		if( FS_stat( fullFilePath, &buf ) != -1 )
		{
			return true;
		}
	}
	return false;
}

// Get the next file, trucking through the path. . don't check for duplicates.
bool CBaseFileSystem::FindNextFileHelper( FindData_t *pFindData )
{
	if ( m_SearchPaths[pFindData->currentSearchPathID].m_bIsPackFile )
	{
		const char *file = SearchPakFile( pFindData->wildCardString.Base(), pFindData->currentSearchPathID, false );

		if ( file )
		{
			char *pFileNameNoPath = (char *)strrchr( file, '\\' );

			if ( pFileNameNoPath )
				file = pFileNameNoPath + 1;

			strcpy( pFindData->findData.cFileName, file );
			FixSlashes( pFindData->findData.cFileName );
		}
	}
	else if ( m_SearchPaths[pFindData->currentSearchPathID].m_bIsRezFile )
	{
		return false;
	}

	// Try the same search path that we were already searching on.
	if( FS_FindNextFile( pFindData->findHandle, &pFindData->findData ) )
	{
		return true;
	}
	pFindData->currentSearchPathID++;

	if ( pFindData->findHandle != INVALID_HANDLE_VALUE )
	{
		FS_FindClose( pFindData->findHandle );
	}
	pFindData->findHandle = INVALID_HANDLE_VALUE;

	while( pFindData->currentSearchPathID < m_SearchPaths.Count() ) 
	{
		if ( pFindData->limitedPathID != -1 )
		{
			if ( pFindData->limitedPathID != m_SearchPaths[pFindData->currentSearchPathID].m_PathID )
				return false;
		}

		// FIXME: Should this work with PAK files?
		if ( m_SearchPaths[pFindData->currentSearchPathID].m_bIsPackFile )
		{
			const char *file = SearchPakFile( pFindData->wildCardString.Base(), pFindData->currentSearchPathID, false );

			if ( file )
			{
				file = strrchr( file, '/' ) + 1;
				strcpy( pFindData->findData.cFileName, file );
				FixSlashes( pFindData->findData.cFileName );
				return true;
			}

			pFindData->currentSearchPathID++;
			continue;
		}

		char *pTmpFileName;
		pTmpFileName = ( char * )_alloca( strlen( m_SearchPaths[pFindData->currentSearchPathID].GetPathString() ) + 
			pFindData->wildCardString.Count() ); // don't need two NULL terminators.
		strcpy( pTmpFileName, m_SearchPaths[pFindData->currentSearchPathID].GetPathString() );
		strcat( pTmpFileName, pFindData->wildCardString.Base() );
		FixSlashes( pTmpFileName );
		
		pFindData->findHandle = FS_FindFirstFile( pTmpFileName, &pFindData->findData );
		if( pFindData->findHandle != INVALID_HANDLE_VALUE )
		{
			return true;
		}
		pFindData->currentSearchPathID++;
	}
	return false;
}	

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : handle - 
// Output : const char
//-----------------------------------------------------------------------------
const char *CBaseFileSystem::FindNext( FileFindHandle_t handle )
{
	FindData_t *pFindData = &m_FindData[handle];

	while( 1 )
	{
		if ( pFindData->m_bIsRezFile )
		{
			return SearchRezFile( NULL, pFindData, false );
		}

		if( FindNextFileHelper( pFindData ) )
		{
			if( !FileInSearchPaths( pFindData->wildCardString.Base(), 
									pFindData->findData.cFileName, 0, pFindData->currentSearchPathID - 1 ) )
			{
				return pFindData->findData.cFileName;
			}
		}
		else
		{
			return NULL;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : handle - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseFileSystem::FindIsDirectory( FileFindHandle_t handle )
{
	FindData_t *pFindData = &m_FindData[handle];

	if ( pFindData->m_bIsRezFile )
		return false;

	return !!( pFindData->findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : handle - 
//-----------------------------------------------------------------------------
void CBaseFileSystem::FindClose( FileFindHandle_t handle )
{
	if ((m_FindData.Count() == 0) || (handle >= m_FindData.Count()))
		return;

	FindData_t *pFindData = &m_FindData[handle];
	Assert(pFindData);

	if ( pFindData->findHandle != INVALID_HANDLE_VALUE)
	{
		FS_FindClose( pFindData->findHandle );
	}
	pFindData->findHandle = INVALID_HANDLE_VALUE;

	pFindData->wildCardString.Purge();

	if ( pFindData->m_bIsRezFile )
	{
		pFindData->m_pRezMatchName.Purge();
		pFindData->m_pRezMatchExt.Purge();
	}

	m_FindData.FastRemove( handle );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
char* CBaseFileSystem::ParseFile( char* pFileBytes, char* pToken, bool* pWasQuoted )
{
	return ::ParseFile(pFileBytes, pToken, pWasQuoted);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pFileName - 
//-----------------------------------------------------------------------------
void CBaseFileSystem::GetLocalCopy( const char *pFileName )
{
	// do nothing. . everything is local.
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pFileName - 
//			*pLocalPath - 
// Output : const char
//-----------------------------------------------------------------------------
const char *CBaseFileSystem::GetLocalPath( const char *pFileName, char *pLocalPath, int localPathBufferSize )
{
	// Is it an absolute path?
	if ( strchr( pFileName, ':' ) )
	{
		strncpy( pLocalPath, pFileName, localPathBufferSize );
		pLocalPath[localPathBufferSize-1] = 0;

		FixSlashes( pLocalPath );
		return pLocalPath;
	}

	struct	_stat buf;
	int i;
	for( i = 0; i < m_SearchPaths.Count(); i++ )
	{
		// FIXME: Should this work with PAK files?
		if ( m_SearchPaths[i].m_bIsPackFile )
		{
			// Search the tree for the filename
			CPackFileEntry search;
			char *temp = (char *)_alloca( strlen( pFileName ) + 1 );
			strcpy( temp, pFileName );
			strlwr( temp );

			FixSlashes( temp );

			search.m_Name = temp;

			int searchresult = m_SearchPaths[i].m_PackFiles.Find( search );

			if ( searchresult != m_SearchPaths[i].m_PackFiles.InvalidIndex() )
			{
				temp = (char *)_alloca( strlen( pFileName ) + 1 );
				strcpy( temp, pFileName );

				_snprintf( pLocalPath, localPathBufferSize - 1, "%s%s", m_SearchPaths[i].m_PackFiles[ searchresult ].m_Name.String(), temp );
				pLocalPath[localPathBufferSize-1] = 0;

				FixSlashes( pLocalPath );
				return pLocalPath;
			}

			continue;
		}
		else if ( m_SearchPaths[i].m_bIsRezFile )
		{
			continue;
		}

		char *pTmpFileName;
		int len = strlen( m_SearchPaths[i].GetPathString() ) + strlen( pFileName );
		pTmpFileName = ( char * )_alloca( len + 1 );
		strcpy( pTmpFileName, m_SearchPaths[i].GetPathString() );
		strcat( pTmpFileName, pFileName );
		FixSlashes( pTmpFileName );
		if( FS_stat( pTmpFileName, &buf ) != -1 )
		{
			strncpy( pLocalPath, pTmpFileName, localPathBufferSize );
			return pLocalPath;
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Returns true on success ( based on current list of search paths, otherwise false if 
//  it can't be resolved )
// Input  : *pFullpath - 
//			*pRelative - 
//			maxRelativePathLength - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseFileSystem::FullPathToRelativePath( const char *pFullpath, char *pRelative )
{
	bool success = false;

	int inlen = strlen( pFullpath );
	if ( inlen <= 0 )
	{
		pRelative[ 0 ] = 0;
		return success;
	}

	strcpy( pRelative, pFullpath );

	char *inpath = (char *)_alloca( inlen + 1 );
	Assert( inpath );
	strcpy( inpath, pFullpath );
	FixSlashes( inpath );
#ifdef _WIN32
	strlwr( inpath );
#endif

	for( int i = 0; i < m_SearchPaths.Count() && !success; i++ )
	{
		// FIXME: Should this work with embedded pak files
		if ( m_SearchPaths[i].m_bIsMapPath )
			continue;

		char *searchbase = new char[ strlen( m_SearchPaths[i].GetPathString() ) + 1 ];
		Assert( searchbase );
		strcpy( searchbase, m_SearchPaths[i].GetPathString() );
		FixSlashes( searchbase );
#ifdef _WIN32
		strlwr( searchbase );
#endif

		if ( !strnicmp( searchbase, inpath, strlen( searchbase ) ) )
		{
			success = true;
			strcpy( pRelative, &inpath[ strlen( searchbase ) ] );
		}

		delete[] searchbase;
	}

	return success;
}


//-----------------------------------------------------------------------------
// Deletes a file
//-----------------------------------------------------------------------------
void CBaseFileSystem::RemoveFile( char const* pRelativePath, const char *pathID )
{
	// Opening for write or append uses Write Path
	ComputeFullWritePath( s_pScratchFileName, pRelativePath, pathID );
	unlink( s_pScratchFileName );
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : **ppdir - 
//-----------------------------------------------------------------------------
bool CBaseFileSystem::GetCurrentDirectory( char* pDirectory, int maxlen )
{
#ifdef _WIN32
	if ( !::GetCurrentDirectoryA( maxlen, pDirectory ) )
#elif _LINUX
	if ( !getcwd( pDirectory, maxlen ) )
#endif
		return false;

	FixSlashes(pDirectory);

	// Strip the last slash
	int len = strlen(pDirectory);
	if ( pDirectory[ len-1 ] == CORRECT_PATH_SEPARATOR )
	{
		pDirectory[ len-1 ] = 0;
	}

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : pfnWarning - warning function callback
//-----------------------------------------------------------------------------
void CBaseFileSystem::SetWarningFunc( void (*pfnWarning)( const char *fmt, ... ) )
{
	m_pfnWarning = pfnWarning;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : level - 
//-----------------------------------------------------------------------------
void CBaseFileSystem::SetWarningLevel( FileWarningLevel_t level )
{
	m_fwLevel = level;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : level - 
//			*fmt - 
//			... - 
//-----------------------------------------------------------------------------
/*void CBaseFileSystem::Warning( FileWarningLevel_t level, const char *fmt, ... )
{
	if ( level > m_fwLevel )
		return;

	va_list argptr; 
    char warningtext[ 4096 ];
    
    va_start( argptr, fmt );
    _vsnprintf( warningtext, sizeof( warningtext ), fmt, argptr );
    va_end( argptr );

	// Dump to stdio
	printf( warningtext );
	if ( m_pfnWarning )
	{
		(*m_pfnWarning)( warningtext );
	}
	else
	{
#ifdef _WIN32
		OutputDebugString( warningtext );
#endif
	}
}*/


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseFileSystem::COpenedFile::COpenedFile( void )
{
	m_pFile = NULL;
	m_pName = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseFileSystem::COpenedFile::~COpenedFile( void )
{
	delete[] m_pName;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : src - 
//-----------------------------------------------------------------------------
CBaseFileSystem::COpenedFile::COpenedFile( const COpenedFile& src )
{
	m_pFile = src.m_pFile;
	if ( src.m_pName )
	{
		m_pName = new char[ strlen( src.m_pName ) + 1 ];
		strcpy( m_pName, src.m_pName );
	}
	else
	{
		m_pName = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : src - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseFileSystem::COpenedFile::operator==( const CBaseFileSystem::COpenedFile& src ) const
{
	return src.m_pFile == m_pFile;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *name - 
//-----------------------------------------------------------------------------
void CBaseFileSystem::COpenedFile::SetName( char const *name )
{
	delete[] m_pName;
	m_pName = new char[ strlen( name ) + 1 ];
	strcpy( m_pName, name );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : char
//-----------------------------------------------------------------------------
char const *CBaseFileSystem::COpenedFile::GetName( void )
{
	return m_pName ? m_pName : "???";
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : src1 - 
//			src2 - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseFileSystem::CSearchPath::PackFileLessFunc( CPackFileEntry const& src1, CPackFileEntry const& src2 )
{
	return ( src1.m_Name < src2.m_Name );
}


const char* CBaseFileSystem::CSearchPath::GetPathString() const
{
	return m_Path.String();
}


const char* CBaseFileSystem::CSearchPath::GetPathIDString() const
{
	return m_PathID.String();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseFileSystem::CSearchPath::CSearchPath( void )
	: m_PackFiles( 0, 32, PackFileLessFunc )
{
	m_Path				= "";
	m_bIsPackFile		= false;
	m_bIsRezFile		= false;
	m_bIsMapPath		= false;
	m_lPackFileTime		= 0L;
	m_nNumPackFiles		= 0;
	m_pRezMgr			= NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseFileSystem::CSearchPath::~CSearchPath( void )
{
	if ( m_bIsPackFile && m_hPackFile )
	{
		// Allow closing to actually occur
		m_fs->m_PackFileHandles.FindAndRemove( m_hPackFile->m_pFile );

		m_fs->Close( (FileHandle_t)m_hPackFile );
	}
	if ( m_bIsRezFile && m_pRezMgr )
	{
		if ( m_pRezMgr->IsOpen() )
		{
			m_pRezMgr->Close();
		}

		delete m_pRezMgr;
	}
}


//-----------------------------------------------------------------------------
// Fixes slashes in the directory name
//-----------------------------------------------------------------------------
static void FixSlashes( char *str )
{
	while( *str )
	{
		if( *str == INCORRECT_PATH_SEPARATOR )
		{
			*str = CORRECT_PATH_SEPARATOR;
		}
		str++;
	}
}

//-----------------------------------------------------------------------------
// Make sure that slashes are of the right kind and that there is a slash at the 
// end of the filename.
// WARNING!!: assumes that you have an extra byte allocated in the case that you need
// a slash at the end.
//-----------------------------------------------------------------------------

static void FixPath( char *str )
{
	char *lastChar = &str[strlen( str ) - 1];
	if( *lastChar != CORRECT_PATH_SEPARATOR && *lastChar != INCORRECT_PATH_SEPARATOR )
	{
		lastChar[1] = CORRECT_PATH_SEPARATOR;
		lastChar[2] = '\0';
	}
	FixSlashes( str );
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *path - 
// Output : static void StripFilename
//-----------------------------------------------------------------------------
static void StripFilename (char *path)
{
	int             length;

	length = strlen(path)-1;
	while (length > 0 && !PATHSEPARATOR(path[length]))
		length--;
	path[length] = 0;
}

