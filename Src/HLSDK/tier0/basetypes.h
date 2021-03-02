#ifndef BASETYPES_H
#define BASETYPES_H

#define DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#define FORWARD_DECLARE_HANDLE(name) typedef struct name##__ *name

template <typename T>
inline T AlignValue( T val, unsigned alignment )
{
	return (T)( ( (uintptr_t)val + alignment - 1 ) & ~( alignment - 1 ) );
}
#ifndef NOTE_UNUSED
#define NOTE_UNUSED(x)	(x = x)	// for pesky compiler / lint warnings
#endif

#endif