#include <metahook.h>

#include "plugins.h"
#include "sys.h"
#include "qgl.h"

#include "pixelformat.h"
#include <png.h>

//----------------------------------------------------------------
// Load TGA file
//----------------------------------------------------------------

#pragma pack(1)

struct TargaHeader
{
	unsigned char id_length, colormap_type, image_type;
	unsigned short colormap_index, colormap_length;
	unsigned char colormap_size;
	unsigned short x_origin, y_origin, width, height;
	unsigned char pixel_size, attributes;
};

#pragma pack()

bool LoadTGA(const char *filename, unsigned char *buffer, int bufferSize, int *wide, int *tall)
{
	FileHandle_t fp = g_pFileSystem->Open(filename, "rb");

	if (!fp)
		return false;

	int columns, rows, numPixels;
	unsigned char *pixbuf;
	int row, column;
	TargaHeader header;

	if (!g_pFileSystem->Read(&header, sizeof(header), fp))
	{
		g_pFileSystem->Close(fp);
		return false;
	}

	if (header.image_type != 2 && header.image_type != 10)
		Sys_Error("[LoadTGA] Only type 2 and 10 targa RGB images supported\n\n%s\n", filename);

	if (header.colormap_type != 0 || (header.pixel_size != 32 && header.pixel_size != 24))
		Sys_Error("[LoadTGA] Only 32 or 24 bit images supported (no colormaps)\n\n%s\n", filename);

	columns = header.width;
	rows = header.height;
	numPixels = columns * rows;

	if (wide)
		*wide = header.width;
	if (tall)
		*tall = header.height;

	if (header.id_length != 0)
		g_pFileSystem->Seek(fp, header.id_length, FILESYSTEM_SEEK_CURRENT);

	int savepos = g_pFileSystem->Tell(fp);
	int filesize = g_pFileSystem->Size(fp);

	int size = filesize - savepos;
	unsigned char *freebuf = (unsigned char *)malloc(size);
	unsigned char *fbuffer = freebuf;
	int readcount = g_pFileSystem->Read(fbuffer, size, fp);
	g_pFileSystem->Close(fp);

	if (!readcount)
	{
		free(freebuf);
		return FALSE;
	}

	if (header.image_type == 2)
	{
		for (row = rows - 1; row >= 0; row--)
		{
			pixbuf = buffer + row * columns * 4;

			for (column = 0; column < columns; column++)
			{
				register unsigned char red, green, blue, alphabyte;

				switch (header.pixel_size)
				{
					case 24:
					{
						blue = fbuffer[0];
						green = fbuffer[1];
						red = fbuffer[2];
						pixbuf[0] = red;
						pixbuf[1] = green;
						pixbuf[2] = blue;
						pixbuf[3] = 255;
						fbuffer += 3;
						pixbuf += 4;
						break;
					}

					case 32:
					{
						blue = fbuffer[0];
						green = fbuffer[1];
						red = fbuffer[2];
						alphabyte = fbuffer[3];
						pixbuf[0] = red;
						pixbuf[1] = green;
						pixbuf[2] = blue;
						pixbuf[3] = alphabyte;
						fbuffer += 4;
						pixbuf += 4;
						break;
					}
				}
			}
		}
	}
	else if (header.image_type == 10)
	{
		register unsigned char red, green, blue, alphabyte;
		unsigned char packetHeader, packetSize, j;

		for (row = rows - 1; row >= 0; row--)
		{
			pixbuf = buffer + row * columns * 4;

			for (column = 0; column < columns; )
			{
				packetHeader = *fbuffer++;
				packetSize = 1 + (packetHeader & 0x7F);

				if (packetHeader & 0x80)
				{
					switch (header.pixel_size)
					{
						case 24:
						{
							blue = fbuffer[0];
							green = fbuffer[1];
							red = fbuffer[2];
							alphabyte = 255;
							fbuffer += 3;
							break;
						}

						case 32:
						{
							blue = fbuffer[0];
							green = fbuffer[1];
							red = fbuffer[2];
							alphabyte = fbuffer[3];
							fbuffer += 4;
							break;
						}
					}

					for (j = 0; j < packetSize; j++)
					{
						pixbuf[0] = red;
						pixbuf[1] = green;
						pixbuf[2] = blue;
						pixbuf[3] = alphabyte;
						pixbuf += 4;
						column++;

						if (column == columns)
						{
							column = 0;

							if (row > 0)
								row--;
							else
								goto breakOut;

							pixbuf = buffer + row * columns * 4;
						}
					}
				}
				else
				{
					for (j = 0; j < packetSize; j++)
					{
						switch (header.pixel_size)
						{
							case 24:
							{
								blue = fbuffer[0];
								green = fbuffer[1];
								red = fbuffer[2];
								pixbuf[0] = red;
								pixbuf[1] = green;
								pixbuf[2] = blue;
								pixbuf[3] = 255;
								fbuffer += 3;
								pixbuf += 4;
								break;
							}

							case 32:
							{
								blue = fbuffer[0];
								green = fbuffer[1];
								red = fbuffer[2];
								alphabyte = fbuffer[3];
								pixbuf[0] = red;
								pixbuf[1] = green;
								pixbuf[2] = blue;
								pixbuf[3] = alphabyte;
								fbuffer += 4;
								pixbuf += 4;
								break;
							}
						}

						column++;

						if (column == columns)
						{
							column = 0;

							if (row > 0)
								row--;
							else
								goto breakOut;

							pixbuf = buffer + row * columns * 4;
						}
					}
				}
			}

			breakOut:;
		}
	}

	free(freebuf);

	return true;
}

//----------------------------------------------------------------
// Load DTX file
//----------------------------------------------------------------

#pragma pack(1)

struct DtxHeader
{
	unsigned int iResType;
	int iVersion;
	unsigned short usWidth;
	unsigned short usHeight;
	unsigned short usMipmaps;
	unsigned short usSections;
	int iFlags;
	int iUserFlags;
	unsigned char ubExtra[12];
	char szCommandString[128];
};

#pragma pack()

bool LoadDTX(const char *filename, unsigned char *buffer, int bufferSize, int *format, int *wide, int *tall, int *size)
{
	FileHandle_t fp;
	DtxHeader hdr;
	int bpp, len, fmt;

	fp = g_pFileSystem->Open(filename, "rb");

	if (!fp)
		return false;

	memset(&hdr, 0, sizeof(hdr));
	g_pFileSystem->Read(&hdr, sizeof(hdr), fp);

	if (hdr.iResType != 0 || hdr.iVersion != -5 || !hdr.usMipmaps)
	{
		Sys_Error("[LoadDTX] Only version -5 images supported\n\n%s\n", filename);
	}

	bpp = !(hdr.ubExtra[2]) ? 3 : hdr.ubExtra[2];

	switch (bpp)
	{
		case 3:
		{
			len = hdr.usWidth * hdr.usHeight * 4;
			fmt = GL_RGBA;
			break;
		}
		case 4:
		{
			len = (hdr.usWidth * hdr.usHeight) >> 1;
			fmt = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
		}
		case 5:
		{
			len = hdr.usWidth * hdr.usHeight;
			fmt = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		}
		case 6:
		{
			len = hdr.usWidth * hdr.usHeight;
			fmt = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		}

		default:
		{
			Sys_Error("[LoadDTX] Only bpp 3/4/5/6 images supported\n\n%s\n", filename);
		}
	}

	if (len > bufferSize)
	{
		g_pFileSystem->Close(fp);
		return false;
	}

	g_pFileSystem->Read(buffer, len, fp);

	if (bpp == 3)
	{
		for (int i = 0; i < len; i += 4)
		{
			buffer[i + 0] ^= buffer[i + 2];
			buffer[i + 2] ^= buffer[i + 0];
			buffer[i + 0] ^= buffer[i + 2];
		}
	}

	if (format)
		*format = fmt;
	if (wide)
		*wide = hdr.usWidth;
	if (tall)
		*tall = hdr.usHeight;
	if (size)
		*size = len;

	g_pFileSystem->Close(fp);

	return true;
}

static FormatMgr g_formatMgr;

bool LoadDTX32(const char *filename, unsigned char *buffer, int bufferSize, int *wide, int *tall)
{
	FileHandle_t fp;
	DtxHeader hdr;
	int bpp, len;
	static byte bits[1024 * 1024 * 4];

	fp = g_pFileSystem->Open(filename, "rb");

	if (!fp)
		return false;

	memset(&hdr, 0, sizeof(hdr));
	g_pFileSystem->Read(&hdr, sizeof(hdr), fp);

	if (hdr.iResType != 0 || hdr.iVersion != -5 || !hdr.usMipmaps)
	{
		Sys_Error("[LoadDTX] Only version -5 images supported\n\n%s\n", filename);
	}

	bpp = !(hdr.ubExtra[2]) ? 3 : hdr.ubExtra[2];

	switch (bpp)
	{
		case 3:
		{
			len = hdr.usWidth * hdr.usHeight * 4;
			break;
		}
		case 4:
		{
			len = (hdr.usWidth * hdr.usHeight) >> 1;
			break;
		}
		case 5:
		{
			len = hdr.usWidth * hdr.usHeight;
			break;
		}
		case 6:
		{
			len = hdr.usWidth * hdr.usHeight;
			break;
		}

		default:
		{
			Sys_Error("[LoadDTX] Only bpp 3/4/5/6 images supported\n\n%s\n", filename);
		}
	}

	if (hdr.usWidth * hdr.usHeight * 4 > bufferSize)
	{
		g_pFileSystem->Close(fp);
		return false;
	}

	g_pFileSystem->Read(bits, len, fp);
	g_pFileSystem->Close(fp);

	if (bpp == BPP_32)
	{
		for (int i = 0; i < len; i += 4)
		{
			buffer[i + 0] = bits[i + 2];
			buffer[i + 1] = bits[i + 1];
			buffer[i + 2] = bits[i + 0];
			buffer[i + 3] = bits[i + 3];
		}
	}
	else
	{
		ConvertRequest q;

		q.m_pSrc = bits;
		q.m_pSrcFormat->Init((BPPIdent)bpp, 0, 0, 0, 0);
		q.m_SrcPitch = 0;

		q.m_pDestFormat->Init(BPP_32, 0xFF000000, 0x000000FF, 0x0000FF00, 0x00FF0000);
		q.m_pDest = buffer;
		q.m_DestPitch = hdr.usWidth * sizeof(DWORD);
		q.m_Width = hdr.usWidth;
		q.m_Height = hdr.usHeight;
		q.m_Flags = 0;

		if (g_formatMgr.ConvertPixels(&q) != LT_OK)
			return false;
	}

	if (wide)
		*wide = hdr.usWidth;
	if (tall)
		*tall = hdr.usHeight;

	return true;
}

bool LoadDTX_Mipmap(const char *filename, unsigned char *buffer, int bufferSize, int *wide, int *tall)
{
	FileHandle_t fp;
	DtxHeader hdr;
	int scaleWidth, scaleHeight;
	int bpp, size, format;
	static byte rgba[1024 * 1024 * 4];

	fp = g_pFileSystem->Open(filename, "rb");

	if (!fp)
		return false;

	memset(&hdr, 0, sizeof(hdr));
	g_pFileSystem->Read(&hdr, sizeof(hdr), fp);

	if (hdr.iResType != 0 || hdr.iVersion != -5 || !hdr.usMipmaps)
	{
		Sys_Error("[LoadDTX] Only version -5 images supported\n\n%s\n", filename);
	}

	bpp = !(hdr.ubExtra[2]) ? BPP_32 : hdr.ubExtra[2];

	scaleWidth = hdr.usWidth;
	scaleHeight = hdr.usHeight;

	for (int i = 0; i < hdr.usMipmaps; i++)
	{
		switch (bpp)
		{
			case 3:
			{
				size = scaleWidth * scaleHeight * 4;
				format = GL_RGBA;
				break;
			}
			case 4:
			{
				size = (scaleWidth * scaleHeight) >> 1;
				format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				break;
			}
			case 5:
			{
				size = scaleWidth * scaleHeight;
				format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				break;
			}
			case 6:
			{
				size = scaleWidth * scaleHeight;
				format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				break;
			}

			default:
			{
				Sys_Error("[LoadDTX] Only bpp 3/4/5/6 images supported\n\n%s\n", filename);
			}
		}

		if (size > bufferSize)
		{
			g_pFileSystem->Close(fp);
			return false;
		}

		g_pFileSystem->Read(buffer, size, fp);

		if (bpp == 3)
		{
			for (int j = 0; j < size; j += 4)
			{
				buffer[j + 0] ^= buffer[j + 2];
				buffer[j + 2] ^= buffer[j + 0];
				buffer[j + 0] ^= buffer[j + 2];
			}
		}

		if (format == GL_RGBA)
		{
			qglTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, scaleWidth, scaleHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		}
		else
		{
			if (qglCompressedTexImage2DARB)
			{
				qglCompressedTexImage2DARB(GL_TEXTURE_2D, i, format, scaleWidth, scaleHeight, 0, size, buffer);
			}
			else
			{
				ConvertRequest request;

				request.m_pSrc = buffer;
				request.m_pSrcFormat->Init((BPPIdent)bpp, 0, 0, 0, 0);
				request.m_SrcPitch = 0;

				request.m_pDestFormat->Init(BPP_32, 0xFF000000, 0x000000FF, 0x0000FF00, 0x00FF0000);
				request.m_pDest = rgba;
				request.m_DestPitch = scaleWidth * sizeof(DWORD);
				request.m_Width = scaleWidth;
				request.m_Height = scaleHeight;
				request.m_Flags = 0;

				if (g_formatMgr.ConvertPixels(&request) == LT_OK)
				{
					qglTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, scaleWidth, scaleHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
				}
			}
		}

		scaleWidth >>= 1;
		scaleHeight >>= 1;
	}

	if (hdr.usMipmaps <= 1)
	{
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		qglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, hdr.usMipmaps - 1);
		qglTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, -1.0);
	}

	if (wide)
		*wide = hdr.usWidth;
	if (tall)
		*tall = hdr.usHeight;

	g_pFileSystem->Close(fp);

	return true;
}

//----------------------------------------------------------------
// Load BMP file
//----------------------------------------------------------------

#define DIB_HEADER_MARKER ((WORD)('M' << 8) | 'B')

bool LoadBMP(const char *szFilename, byte *buffer, int bufferSize, int *wide, int *tall)
{
	FileHandle_t file = g_pFileSystem->Open(szFilename, "rb");

	if (!file)
		return false;

	BITMAPFILEHEADER bmfHeader;
	LPBITMAPINFO lpbmi;
	DWORD dwFileSize = g_pFileSystem->Size(file);

	if (!g_pFileSystem->Read(&bmfHeader, sizeof(bmfHeader), file))
	{
		g_pFileSystem->Close(file);
		return false;
	}

	if (bmfHeader.bfType == DIB_HEADER_MARKER)
	{
		DWORD dwBitsSize = dwFileSize - sizeof(bmfHeader);

		unsigned char *pDIB = (unsigned char *)malloc(dwBitsSize);

		if (!g_pFileSystem->Read(pDIB, dwBitsSize, file))
		{
			free(pDIB);
			g_pFileSystem->Close(file);
			return false;
		}

		lpbmi = (LPBITMAPINFO)pDIB;

		if (lpbmi->bmiHeader.biBitCount != 8)
			Sys_Error("[LoadBMP] Only 8 bit images supported\n\n%s\n", szFilename);

		if (wide)
			*wide = lpbmi->bmiHeader.biWidth;

		if (tall)
			*tall = lpbmi->bmiHeader.biHeight;

		unsigned char *rgba = (unsigned char *)(pDIB + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));

		for (int j = 0; j < lpbmi->bmiHeader.biHeight; j++)
		{
			for (int i = 0; i < lpbmi->bmiHeader.biWidth; i++)
			{
				int y = (lpbmi->bmiHeader.biHeight - j - 1);

				int offs = (y * lpbmi->bmiHeader.biWidth + i);
				int offsdest = (j * lpbmi->bmiHeader.biWidth + i) * 4;
				unsigned char *src = rgba + offs;
				unsigned char *dst = buffer + offsdest;

				dst[0] = lpbmi->bmiColors[*src].rgbRed;
				dst[1] = lpbmi->bmiColors[*src].rgbGreen;
				dst[2] = lpbmi->bmiColors[*src].rgbBlue;
				dst[3] = 255;
			}
		}

		free(pDIB);
	}

	g_pFileSystem->Close(file);

	return true;
}

//----------------------------------------------------------------
// Load PNG file
//----------------------------------------------------------------

bool LoadPNG(const char *filename, unsigned char *buffer, int bufferSize, int *wide, int *tall)
{
	FileHandle_t fp = g_pFileSystem->Open(filename, "rb");

	if (!fp)
		return false;

	size_t size;

	g_pFileSystem->Seek(fp, 0, FILESYSTEM_SEEK_TAIL);
	size = g_pFileSystem->Tell(fp);
	g_pFileSystem->Seek(fp, 0, FILESYSTEM_SEEK_HEAD);

	if (!size)
	{
		g_pFileSystem->Close(fp);
		return false;
	}

	unsigned char *_Buffer = (unsigned char *)malloc(size);

	g_pFileSystem->Read(_Buffer, size, fp);
	g_pFileSystem->Close(fp);

	png_image image;

	memset(&image, 0, sizeof(image));
	image.version = PNG_IMAGE_VERSION;

	bool result = false;

	if (png_image_begin_read_from_memory(&image, _Buffer, size))
	{
		image.format = PNG_FORMAT_RGBA;

		if (png_image_finish_read(&image, NULL, buffer, 0, NULL))
		{
			if (wide)
				*wide = image.width;

			if (tall)
				*tall = image.height;

			result = true;
		}
	}

	free(_Buffer);

	return result;
}