#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdio.h>
#include <new.h>

#define ALIGN16
#define FORCEINLINE __forceinline
#define RESTRICT __restrict
#define DECL_ALIGN(x) __declspec(align(x))
#define FASTCALL __fastcall

#define PLATFORM_INTERFACE

#define abstract_class class

typedef unsigned char byte;
typedef unsigned char uint8;
typedef __int16 int16;
typedef __int64 int64;
typedef unsigned __int64 uint64;
typedef int int32;
typedef unsigned int uint32;

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef NULL
#define NULL 0
#endif

#if _MSC_VER <= 1400
#ifndef offsetof
#define offsetof(s,m)   (size_t)&reinterpret_cast<const volatile char&>((((s *)0)->m))
#endif
#endif

template <class T>
inline void Construct( T* pMemory )
{
	::new( pMemory ) T;
}

template <class T>
inline void CopyConstruct( T* pMemory, T const& src )
{
	::new( pMemory ) T(src);
}

template <class T>
inline void Destruct( T* pMemory )
{
	pMemory->~T();

#ifdef _DEBUG
	memset( pMemory, 0xDD, sizeof(T) );
#endif
}

inline bool IsPC(void) { return true; }
inline bool IsX360(void) { return false; }
inline bool IsXbox(void) { return false; }

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

#ifndef min
#define min(x, y) ((x) > (y) ? (y) : (x))
#endif
#ifndef max
#define max(x, y) ((x) < (y) ? (y) : (x))
#endif

inline bool IsPowerOfTwo( int value )
{
	return (value & ( value - 1 )) == 0;
}

inline float RemapVal( float val, float A, float B, float C, float D)
{
	if ( A == B )
		return val >= B ? D : C;
	return C + (D - C) * (val - A) / (B - A);
}

#endif