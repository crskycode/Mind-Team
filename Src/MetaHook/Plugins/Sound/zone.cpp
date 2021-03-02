
#include <metahook.h>
#include "engfuncs.h"
#include "zone.h"

void *Cache_Check(cache_user_t *c)
{
	return g_pfn_Cache_Check(c);
}

void *Cache_Alloc(cache_user_t *c, int size, char *name)
{
	return g_pfn_Cache_Alloc(c, size, name);
}

void Cache_Free(cache_user_t *c)
{
	return g_pfn_Cache_Free(c);
}