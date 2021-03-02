#ifndef ZONE_H
#define ZONE_H

typedef struct cache_user_s
{
	void	*data;
} cache_user_t;


void *Cache_Check(cache_user_t *c);
void *Cache_Alloc(cache_user_t *c, int size, char *name);
void Cache_Free(cache_user_t *c);

#endif