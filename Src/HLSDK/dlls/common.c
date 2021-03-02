/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
****/

char com_token[1024];

char *MP_COM_Parse(char *data)
{
	int c;
	int len;

	len = 0;
	com_token[0] = 0;

	if (!data)
		return 0;

skipwhite:
	while ((c = *data) <= ' ')
	{
		if (c == 0)
		{
			return 0;	// end of file;
		}
		data++;
	}

	if (c=='/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;
		goto skipwhite;
	}

	if (c == '\"')
	{
		data++;
		do
		{
			c = *data++;
			if (c == '\"')
			{
				com_token[len] = 0;
				return data;
			}
			com_token[len] = c;
			len++;
		} while (1);
	}

	if (c == '{' || c == '}'|| c == ')'|| c == '(' || c == '\'' || c == ':')
	{
		com_token[len] = c;
		len++;
		com_token[len] = 0;
		return data + 1;
	}

	do
	{
		com_token[len] = c;
		data++;
		len++;
		c = *data;
		if (c == '{' || c == '}'|| c == ')'|| c == '(' || c == '\'' || c == ':')
			break;
	} while (c > 32);

	com_token[len] = 0;
	return data;
}

char *MP_COM_GetToken(void)
{
	return com_token;
}