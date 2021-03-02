#include "IKeyValuesSystem.h"

IKeyValuesSystem *g_pKeyValuesSystem;

IKeyValuesSystem *KeyValuesSystem(void)
{
	return g_pKeyValuesSystem;
}