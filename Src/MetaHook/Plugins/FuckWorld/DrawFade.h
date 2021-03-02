#ifndef DRAWFADE_H
#define DRAWFADE_H

typedef struct fade_api_s
{
	int		( *StartupTextFade )	( int x, int y, bool center, int r, int g, int b, int a, float in, float hold, float out, float next, bool repeat, int font, bool outline, const wchar_t *text, int channel );
	void	( *RemoveTextFade )		( int channel );
	void	( *DrawTextFade )		( void );
	int		( *StartupTextureFade )	( int x, int y, int r, int g, int b, int a, float in, float hold, float out, float next, bool repeat, const char *name, int channel );
	void	( *RemoveTextureFade )	( int channel );
	void	( *DrawTextureFade )	( void );
}
fade_api_t;

extern fade_api_t gFadeAPI;

#endif