#ifndef __LTBASEDEFS_H__
#define __LTBASEDEFS_H__

#ifndef __STDLIB_H__
#include <stdlib.h>
#define __STDLIB_H__
#endif

#ifndef __STRING_H__
#include <string.h>
#define __STRING_H__
#endif

#ifndef __STDARG_H__
#include <stdarg.h>
#define __STDARG_H__
#endif

#ifndef __STDIO_H__
#include <stdio.h>
#define __STDIO_H__
#endif

#ifndef __CTYPE_H__
#include <ctype.h>
#define __CTYPE_H__
#endif

#ifndef __ASSERT_H__
#include <assert.h>
#define __ASSERT_H__
#endif

#ifndef __BASELIST_H__
#include "baselist.h"
#endif

#ifndef __BASEHASH_H__
#include "basehash.h"
#endif

#ifndef __VIRTLIST_H__
#include "virtlist.h"
#endif

#pragma warning(disable:4996)

typedef int BOOL;
typedef unsigned char BYTE;
typedef unsigned int uint32;

#define TRUE 1
#define FALSE 0

#define ASSERT assert

#endif