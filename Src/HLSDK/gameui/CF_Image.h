#ifndef CF_IMAGE_H
#define CF_IMAGE_H

#include <vgui_controls/Panel.h>

// ITextureAPI.h
typedef struct tex_s tex_t;

namespace vgui
{

class CFImage : public Panel
{
	DECLARE_CLASS_SIMPLE(CFImage, Panel);

public:
	CFImage(Panel *parent, const char *panelName);

	virtual void SetImage(const char *fileName, ...);

	virtual void SetClip(int x, int y, int wide, int tall);
	virtual void GetClip(int &x, int &y, int &wide, int &tall);

protected:
	virtual void PaintBackground(void);
	virtual void Paint(void);
	virtual void PaintBorder(void);
	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void ApplySchemeSettings(IScheme *pScheme);

private:
	tex_t *	_image;
	int		_alpha;
	int		_clip[4];
};

}

#endif