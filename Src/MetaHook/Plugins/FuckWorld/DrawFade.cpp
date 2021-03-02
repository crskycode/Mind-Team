#include <metahook.h>
#include "DrawFonts.h"
#include "DrawTextures.h"
#include "DrawFade.h"

bool textfade_initialize = false;

struct textfade_t
{
	float start;
	float in;
	float hold;
	float out;
	float next;
	int x, y;
	int font;
	int r, g, b, a;
	wchar_t *text;
	int lenght;
	bool repeat;
};

textfade_t g_textFade[64];

int StartupTextFade(int x, int y, bool center, int r, int g, int b, int a, float in, float hold, float out, float next, bool repeat, int font, bool outline, const wchar_t *text, int channel)
{
	if (!textfade_initialize)
		return -1;

	if (!text || !*text)
		return -1;

	if (channel < -1 || channel > 63)
		return -1;

	if (channel == -1)
	{
		for (int i = 0; i < 64; i++)
		{
			if (!g_textFade[i].text)
			{
				channel = i;
				break;
			}
		}

		if (channel == -1)
			return -1;
	}

	textfade_t &item = g_textFade[channel];

	if (item.text)
	{
		delete[] item.text;
		item.text = NULL;
	}

	if (center)
	{
		int w, t;

		gFontAPI.GetTextSize(font, text, w, t);

		x -= w / 2;
		y -= t / 2;
	}

	item.start = gEngfuncs.GetClientTime();
	item.in = in;
	item.hold = hold;
	item.out = out;
	item.next = next;
	item.x = x;
	item.y = y;
	item.font = font;
	item.r = r;
	item.g = g;
	item.b = b;
	item.a = a;
	item.repeat = repeat;
	item.lenght = wcslen(text);
	item.text = new wchar_t[item.lenght + 1];

	wcscpy(item.text, text);

	return channel;
}

void RemoveTextFade(int channel)
{
	if (!textfade_initialize)
		return;

	if (channel < -1 || channel > 63)
		return;

	if (channel == -1)
	{
		for (int i = 0; i < 64; i++)
		{
			textfade_t &item = g_textFade[i];

			if (!item.text)
				continue;

			delete[] item.text;
			item.text = NULL;
		}

		return;
	}

	textfade_t &item = g_textFade[channel];

	if (item.text)
	{
		delete[] item.text;
		item.text = NULL;
	}
}

void DrawTextFade(void)
{
	if (!textfade_initialize)
	{
		memset(g_textFade, 0, sizeof(g_textFade));
		textfade_initialize = true;
	}

	float time = gEngfuncs.GetClientTime();

	for (int i = 0; i < 64; i++)
	{
		textfade_t &item = g_textFade[i];

		if (!item.text)
			continue;

calcalpha:
		int alpha = 255;

		if (item.start > time)
		{
			alpha = 0;
		}
		else if (time > item.start && time < item.start + item.in)
		{
			alpha = (time - item.start) / item.in * (float)item.a;
		}
		else if (time > item.start + item.in && time < item.start + item.in + item.hold)
		{
			alpha = 255;
		}
		else if (time > item.start + item.in + item.hold && time < item.start + item.in + item.hold + item.out)
		{
			alpha = (item.start + item.in + item.hold + item.out - time) / item.out * (float)item.a;
		}
		else if (time > item.start + item.in + item.hold + item.out)
		{
			if (item.repeat)
			{
				item.start = time + item.next - 0.01;
				goto calcalpha;
			}
			else
			{
				delete[] item.text;
				item.text = NULL;
				continue;
			}
		}

		if (alpha < 0)
			alpha = 0;
		else if (alpha > 255)
			alpha = 255;

		gFontAPI.DrawSetTextFont(item.font);
		gFontAPI.DrawSetTextColor(item.r, item.g, item.b, alpha);
		gFontAPI.DrawSetTextPos(item.x, item.y);
		gFontAPI.DrawPrintText(item.text);
	}
}

bool texturefade_initialize = false;

struct texturefade_t
{
	float start;
	float in;
	float hold;
	float out;
	float next;
	int x, y;
	int r, g, b, a;
	char *name;
	bool repeat;
};

texturefade_t g_textureFade[64];

int StartupTextureFade(int x, int y, int r, int g, int b, int a, float in, float hold, float out, float next, bool repeat, const char *name, int channel)
{
	if (!texturefade_initialize)
		return -1;

	if (!name || !*name)
		return -1;

	if (channel < -1 || channel > 63)
		return -1;

	if (channel == -1)
	{
		for (int i = 0; i < 64; i++)
		{
			if (!g_textureFade[i].name)
			{
				channel = i;
				break;
			}
		}

		if (channel == -1)
			return -1;
	}

	texturefade_t &item = g_textureFade[channel];

	if (item.name)
	{
		delete[] item.name;
		item.name = NULL;
	}

	item.start = gEngfuncs.GetClientTime();
	item.in = in;
	item.hold = hold;
	item.out = out;
	item.next = next;
	item.x = x;
	item.y = y;
	item.r = r;
	item.g = g;
	item.b = b;
	item.a = a;
	item.repeat = repeat;

	item.name = new char[strlen(name) + 1];
	strcpy(item.name, name);

	return channel;
}

void RemoveTextureFade(int channel)
{
	if (!texturefade_initialize)
		return;

	if (channel < -1 || channel > 63)
		return;

	if (channel == -1)
	{
		for (int i = 0; i < 64; i++)
		{
			texturefade_t &item = g_textureFade[i];

			if (!item.name)
				continue;

			delete[] item.name;
			item.name = NULL;
		}

		return;
	}

	texturefade_t &item = g_textureFade[channel];

	if (item.name)
	{
		delete[] item.name;
		item.name = NULL;
	}
}

void DrawTextureFade(void)
{
	if (!texturefade_initialize)
	{
		memset(g_textureFade, 0, sizeof(g_textureFade));
		texturefade_initialize = true;
	}

	float time = gEngfuncs.GetClientTime();

	for (int i = 0; i < 64; i++)
	{
		texturefade_t &item = g_textureFade[i];

		if (!item.name)
			continue;

calcalpha:
		int alpha = 255;

		if (item.start > time)
		{
			alpha = 0;
		}
		else if (time > item.start && time < item.start + item.in)
		{
			alpha = (time - item.start) / item.in * (float)item.a;
		}
		else if (time > item.start + item.in && time < item.start + item.in + item.hold)
		{
			alpha = 255;
		}
		else if (time > item.start + item.in + item.hold && time < item.start + item.in + item.hold + item.out)
		{
			alpha = (item.start + item.in + item.hold + item.out - time) / item.out * (float)item.a;
		}
		else if (time > item.start + item.in + item.hold + item.out)
		{
			if (item.repeat)
			{
				item.start = time + item.next - 0.01;
				goto calcalpha;
			}
			else
			{
				delete[] item.name;
				item.name = NULL;
				continue;
			}
		}

		if (alpha < 0)
			alpha = 0;
		else if (alpha > 255)
			alpha = 255;

		tex_t *texture = gTexAPI.Load(item.name);

		gTexAPI.RenderMode(RenderNormal);
		gTexAPI.DrawSetTexture(texture);
		gTexAPI.DrawSetColor(item.r, item.g, item.b, alpha);
		gTexAPI.DrawTexturedRect(item.x, item.y, texture->wide, texture->tall);
	}
}

fade_api_t gFadeAPI = 
{
	StartupTextFade,
	RemoveTextFade,
	DrawTextFade,
	StartupTextureFade,
	RemoveTextureFade,
	DrawTextureFade,
};