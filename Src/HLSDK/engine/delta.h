/***
 *
 * Copyright (c ) 1996-2002, Valve LLC. All rights reserved.
 *
 * This product contains software technology licensed from Id
 * Software, Inc. ("Id Technology" ). Id Technology (c ) 1996 Id Software, Inc.
 * All Rights Reserved.
 *
 * Use, distribution, and modification of this source code and/or resulting
 * object code is restricted to non-commercial enhancements to products from
 * Valve LLC. All other use, distribution, or modification is prohibited
 * without written permission from Valve LLC.
 *
***/

#ifndef DELTA_H
#define DELTA_H

#define DF_SEND				0x00000001
#define DT_BYTE				0x00000001
#define DT_SHORT			0x00000002
#define DT_FLOAT			0x00000004
#define DT_INTEGER			0x00000008
#define DT_ANGLE			0x00000010
#define DT_TIMEWINDOW_8		0x00000020
#define DT_TIMEWINDOW_BIG	0x00000040
#define DT_STRING			0x00000080
#define DT_SIGNED			0x80000000

typedef struct delta_stats_s
{
	int		sendcount;
	int		receivedcount;

} delta_stats_t;

typedef struct delta_description_s
{
	int				fieldType;
	char			fieldName[32];
	int				fieldOffset;
	int				fieldSize;
	int				significant_bits;
	float			premultiply;
	float			postmultiply;
	short			flags;
	delta_stats_t	stats;

} delta_description_t;

typedef void ( *encoder_t )( struct delta_s *pFields, const unsigned char *from, const unsigned char *to );

typedef struct delta_s
{
	qboolean	dynamic;
	int			fieldCount;
	char		conditionalencodename[32];
	encoder_t	conditionalencode;
	struct delta_description_s	*pdd;

} delta_t;

typedef struct delta_encoder_s
{
	struct delta_encoder_s	*next;
	char		*name;
	encoder_t	conditionalencode;

} delta_encoder_t;

#endif