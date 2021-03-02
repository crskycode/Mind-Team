
#include "extdll.h"
#include "util.h"
#include "filesystem_simple.h"
#include <sys/stat.h>

CFileSystem_Simple g_FileSystem;

CFileSystem_Simple *filesystem = &g_FileSystem;


FileHandle_t CFileSystem_Simple::Open( const char *filename, const char *mode, const char *pathID )
{
	char gamedir[64];
	char fullpath[260];

	GET_GAME_DIR( gamedir );
	sprintf( fullpath, "%s/%s", gamedir, filename );

	return (FileHandle_t)fopen( fullpath, mode );
}

void CFileSystem_Simple::Close(FileHandle_t file)
{
	fclose( (FILE *)file );
}

int CFileSystem_Simple::Write(const void *input, int size, FileHandle_t file)
{
	return (int)fwrite( input, 1, size, (FILE *)file );
}

int CFileSystem_Simple::Read(void *output, int size, FileHandle_t file)
{
	return (int)fread( output, 1, size, (FILE *)file );
}

int CFileSystem_Simple::Size(const char *filename)
{
	char gamedir[64];
	char fullpath[260];
	struct _stat statbuf;

	GET_GAME_DIR( gamedir );
	sprintf( fullpath, "%s/%s", gamedir, filename );

	if ( _stat( fullpath, &statbuf ) != 0 )
		return 0;

	return statbuf.st_size;
}