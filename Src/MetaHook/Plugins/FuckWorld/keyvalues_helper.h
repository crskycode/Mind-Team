#ifndef KEYVALUES_HELPER_H
#define KEYVALUES_HELPER_H

#include <newkeyvalues.h>

inline NewKeyValues *LoadKeyValuesFromFile( const char *pFileName )
{
	char	*pBuffer;

	NewKeyValues	*pResult = NULL;

	pBuffer = (char *)gEngfuncs.COM_LoadFile( pFileName, 5, NULL );

	if ( pBuffer )
	{
		NewKeyValues	*pDat;

		pDat = new NewKeyValues("Root");

		if ( pDat )
		{
			if ( pDat->LoadFromBuffer( pBuffer ) )
			{
				pResult = pDat;
			}
		}

		gEngfuncs.COM_FreeFile( pBuffer );
	}

	return pResult;
}

#endif