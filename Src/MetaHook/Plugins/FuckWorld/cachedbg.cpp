
#include <metahook.h>

char *CommatizeNumber(int num, char *pout)
{
	int len = 0;
	int i;
	char outbuf[50];

	memset(outbuf, 0, sizeof(outbuf));

	while (num)
	{
		char tempbuf[50];
		int temp = num % 1000;

		num = num / 1000;
		strcpy(tempbuf, outbuf);

		_snprintf(outbuf, sizeof(outbuf), ",%03i%s", temp, tempbuf);
	}

	len = strlen(outbuf);

	for (i = 0; i < len; i++)
	{
		if (outbuf[i] != '0' &&  outbuf[i] != ',')
		{
			break;
		}
	}

	if (i == len)
	{
		strcpy(pout, "0");
	}
	else
	{
		strcpy(pout, &outbuf[i]);
	}

	return pout;
}

typedef struct cache_system_s
{
	int						size;
	void					*user;
	char					name[64];
	struct cache_system_s	*prev, *next;
	struct cache_system_s	*lru_prev, *lru_next;

} cache_system_t;

// !3266
cache_system_t *cache_head = (cache_system_t *)0x024F9D60;

void (*g_pfnHunk_Print)(qboolean all) = (void (*)(qboolean))0x01DCF4D0;

void (*g_pfnCache_Print)(void) = (void (*)(void))0x01DCFCA0;
void (*g_pfnCache_Print_Models_And_Totals)(void) = (void (*)(void))0x01DD00D0;
void (*g_pfnCache_Print_Sounds_And_Totals)(void) = (void (*)(void))0x01DD01D0;

static int CacheSystemCompare(const void *ppcs1, const void *ppcs2)
{
	cache_system_t *pcs1 = *(cache_system_t **)ppcs1;
	cache_system_t *pcs2 = *(cache_system_t **)ppcs2;

	return stricmp(pcs1->name, pcs2->name);
}

void Hunk_Print_f(void)
{
	g_pfnHunk_Print(1);
}

void Cache_Print_f(void)
{
	//cache_system_t *cd;
	//cache_system_t *sortarray[512];
	//int i, j;
	//char buf[50];
	//int modelscount, modelsbytes, soundscount, soundsbytes;

	//memset(sortarray, 0, sizeof(sortarray));

	//gEngfuncs.Con_Printf("\nCACHE:\n");

	//for (i = 0, cd = cache_head->next; cd != cache_head; cd = cd->next)
	//{
	//	sortarray[i++] = cd;
	//}

	//qsort(sortarray, i, sizeof(cache_system_t *), CacheSystemCompare);

	//modelscount = modelsbytes = 0;
	//soundscount = soundsbytes = 0;

	//for (j = 0; j < i; j++)
	//{
	//	if (strstr(sortarray[j]->name, ".mdl"))
	//	{
	//		modelscount++;
	//		modelsbytes += sortarray[j]->size;
	//	}
	//	else if (strstr(sortarray[j]->name, ".wav"))
	//	{
	//		soundscount++;
	//		soundsbytes += sortarray[j]->size;
	//	}

	//	gEngfuncs.Con_Printf("%16.16s : %-16s\n", CommatizeNumber(sortarray[j]->size, buf), sortarray[j]->name);
	//}

	//gEngfuncs.Con_Printf("Total models: %d, bytes: %d\n", modelscount, modelsbytes);
	//gEngfuncs.Con_Printf("Total sounds: %d, bytes: %d\n", soundscount, soundsbytes);

	// !3266
	g_pfnCache_Print();
}

void Cache_Print_Models_And_Totals_f(void)
{
	// !3266
	g_pfnCache_Print_Models_And_Totals();
}

void Cache_Print_Sounds_And_Totals_f(void)
{
	// !3266
	g_pfnCache_Print_Sounds_And_Totals();
}

void CacheDbg_Init(void)
{
	gEngfuncs.pfnAddCommand("h_print", Hunk_Print_f);
	gEngfuncs.pfnAddCommand("c_print", Cache_Print_f);
	gEngfuncs.pfnAddCommand("c_print_models", Cache_Print_Models_And_Totals_f);
	gEngfuncs.pfnAddCommand("c_print_sounds", Cache_Print_Sounds_And_Totals_f);
}