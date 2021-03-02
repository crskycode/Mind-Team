#ifndef DBG_H
#define DBG_H

#include <assert.h>

#define Assert assert
#define AssertOnce assert
#define AssertValidStringPtr(...) ((void)0)
#define AssertValidReadPtr(...) ((void)0)
#define AssertValidWritePtr(...) ((void)0)

#define Error(...) ((void)0)
#define Warning(...) ((void)0)
#define Msg(...) ((void)0)
#define DevMsg(...) ((void)0)

#define VPROF(...) ((void)0)
#define VerifyEquals(...) ((void)0)

#define assert_cast static_cast

#define stackalloc _alloca

#define MEM_ALLOC_CREDIT_CLASS() ((void)0)
#define MEM_ALLOC_CREDIT() ((void)0)
#define MEM_ALLOC_CREDIT_(...) ((void)0)
#define MEM_ALLOC_CLASSNAME(type) NULL
#define COMPILE_TIME_ASSERT(...) ((void)0)

#define DebuggerBreak() assert(0);

#endif