
#include <windows.h>

BOOL WildcardMatch(char *lpFilename, char *lpWildcard)
{
	int nLeftLen = strlen(lpFilename);
	int nRightLen = strlen(lpWildcard);

	BOOL mapMatch[MAX_PATH][MAX_PATH];

	memset(mapMatch, 0, sizeof(mapMatch));
	mapMatch[0][0] = TRUE;

	for (int iLeft = 1; iLeft <= nLeftLen; iLeft++)
	{
		for (int iRight = 1; iRight <= nRightLen; iRight++)
		{
			if (mapMatch[iLeft - 1][iRight - 1])
			{
				if (lpFilename[iLeft - 1] == lpWildcard[iRight - 1] || lpWildcard[iRight - 1] == '?')
				{
					mapMatch[iLeft][iRight] = TRUE;

					if (iLeft == nLeftLen && iRight < nRightLen)
					{
						for (int iRemain = iRight + 1; iRemain <= nRightLen; iRemain++)
						{
							if ('*' == lpWildcard[iRemain - 1])
							{
								mapMatch[iLeft][iRemain] = TRUE;
							}
							else
							{
								break;
							}
						}
					}
				}
				else if (lpWildcard[iRight - 1] == '*')
				{
					for (int iRemain = iLeft - 1; iRemain <= nLeftLen; iRemain++)
					{
						mapMatch[iRemain][iRight] = TRUE;
					}
				}
			}
		}

		BOOL bFound = FALSE;

		for (int iRemain = 1; iRemain <= nRightLen; iRemain++)
		{
			if (mapMatch[iLeft][iRemain])
			{
				bFound  = TRUE;
				break;
			}
		}

		if (!bFound)
		{
			return FALSE;
		}
	}

	return mapMatch[nLeftLen][nRightLen];
}