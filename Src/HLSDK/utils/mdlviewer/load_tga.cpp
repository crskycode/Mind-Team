#include <stdio.h>
#include <malloc.h>
#include <sys/stat.h>

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
	FILE *fp = fopen(filename, "rb");

	if (!fp)
		return false;

	int columns, rows, numPixels;
	unsigned char *pixbuf;
	int row, column;
	TargaHeader header;

	if (fread(&header, sizeof(header), 1, fp) != 1)
	{
		fclose(fp);
		return false;
	}

	if (header.image_type != 2 && header.image_type != 10)
		return false;

	if (header.colormap_type != 0 || (header.pixel_size != 32 && header.pixel_size != 24))
		return false;

	columns = header.width;
	rows = header.height;
	numPixels = columns * rows;

	if (wide)
		*wide = header.width;
	if (tall)
		*tall = header.height;

	if (header.id_length != 0)
		fseek(fp, header.id_length, SEEK_CUR);

	int savepos = ftell(fp);
	struct stat _stat;
	stat(filename, &_stat);
	int filesize = _stat.st_size;

	int size = filesize - savepos;
	unsigned char *freebuf = (unsigned char *)malloc(size);
	unsigned char *fbuffer = freebuf;
	int readcount = fread(fbuffer, 1, size, fp);
	fclose(fp);

	if (!readcount)
	{
		free(freebuf);
		return false;
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