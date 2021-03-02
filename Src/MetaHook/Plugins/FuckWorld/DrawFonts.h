#ifndef DRAWFONTS_H
#define DRAWFONTS_H

#define FW_DONTCARE			0
#define FW_THIN				100
#define FW_EXTRALIGHT		200
#define FW_LIGHT			300
#define FW_NORMAL			400
#define FW_MEDIUM			500
#define FW_SEMIBOLD			600
#define FW_BOLD				700
#define FW_EXTRABOLD		800
#define FW_HEAVY			900

#ifndef FONTFLAG
#define FONTFLAG

enum FontFlag
{
	FONTFLAG_NONE			= 0x000,
	FONTFLAG_ITALIC			= 0x001,
	FONTFLAG_UNDERLINE		= 0x002,
	FONTFLAG_STRIKEOUT		= 0x004,
	FONTFLAG_SYMBOL			= 0x008,
	FONTFLAG_ANTIALIAS		= 0x010,
	FONTFLAG_OUTLINE		= 0x200,
};

#endif

typedef struct font_api_s
{
	int		( *CreateFont )				( void );
	bool	( *AddGlyphSetToFont )		( int font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags );
	bool	( *AddCustomFontFile )		( const char *fontFileName );
	int		( *GetFontTall )			( int font );
	void	( *GetCharABCwide )			( int font, int ch, int &a, int &b, int &c );
	int		( *GetCharacterWidth )		( int font, int ch );
	void	( *GetTextSize )			( int font, const wchar_t *text, int &wide, int &tall );
	int		( *GetFontAscent )			( int font, wchar_t wch );
	void	( *DrawSetTextFont )		( int font );
	void	( *DrawSetTextColor )		( int r, int g, int b, int a );
	void	( *DrawSetTextPos )			( int x, int y );
	void	( *DrawGetTextPos )			( int &x,int &y );
	void	( *DrawPrintText )			( const wchar_t *text );
	void	( *DrawCenterText )			( const wchar_t *text );
}
font_api_t;

extern font_api_t gFontAPI;

#endif