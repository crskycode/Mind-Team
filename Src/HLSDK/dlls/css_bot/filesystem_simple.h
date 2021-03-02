
#ifndef FILESYSTEM_SIMPLE_H
#define FILESYSTEM_SIMPLE_H

typedef void *FileHandle_t;

class CFileSystem_Simple
{
public:
	FileHandle_t Open(const char *filename, const char *mode, const char *pathID = 0);
	void Close(FileHandle_t file);
	int Read(void *output, int size, FileHandle_t file);
	int Write(const void *input, int size, FileHandle_t file);
	int Size(const char *filename);
};

extern CFileSystem_Simple *filesystem;

#endif