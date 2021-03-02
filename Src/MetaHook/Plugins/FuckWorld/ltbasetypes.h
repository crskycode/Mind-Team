//------------------------------------------------------------------
//
//	FILE	  : BDEFS.H
//
//	PURPOSE	  : Base definitions header file
//
//	CREATED	  : 1st May 1996
//
//	COPYRIGHT : Microsoft 1996 All Rights Reserved
//
//------------------------------------------------------------------

#ifndef __BDEFS_H__
#define __BDEFS_H__

#include <windows.h>

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;

typedef int LTBOOL;
typedef int LTRESULT;

#define LTTRUE 1
#define LTFALSE 0

#include <assert.h>

#define ASSERT assert

enum
{
	LT_OK			= 0,
	LT_ERROR		= 1,
	LT_UNSUPPORTED	= 83,
};

#define LTNULL 0

#endif