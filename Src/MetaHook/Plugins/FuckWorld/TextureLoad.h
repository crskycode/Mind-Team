#ifndef TEXTURELOAD_H
#define TEXTURELOAD_H

bool LoadTGA(const char *filename, unsigned char *buffer, int bufferSize, int *wide, int *tall);
bool LoadDTX(const char *filename, unsigned char *buffer, int bufferSize, int *format, int *wide, int *tall, int *size);
bool LoadDTX32(const char *filename, unsigned char *buffer, int bufferSize, int *wide, int *tall);
bool LoadDTX_Mipmap(const char *filename, unsigned char *buffer, int bufferSize, int *wide, int *tall);
bool LoadBMP(const char *szFilename, byte *buffer, int bufferSize, int *wide, int *tall);
bool LoadPNG(const char *filename, unsigned char *buffer, int bufferSize, int *wide, int *tall);

#endif