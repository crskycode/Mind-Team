#include <stdio.h>
#include <objbase.h>

const char *CreateGuid(void)
{
	GUID guid;
	static char string[40];

	if (CoCreateGuid(&guid) != S_OK)
		return "";

	sprintf(string, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid.Data1,
		guid.Data2,
		guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	return string;
}