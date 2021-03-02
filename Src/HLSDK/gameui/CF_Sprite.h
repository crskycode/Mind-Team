#ifndef CF_SPRITE_H
#define CF_SPRITE_H

#include <vgui_controls/Panel.h>
#include <utlvector.h>

// ITextureAPI.h
typedef struct tex_s tex_t;

namespace vgui
{

class CFSprite : public Panel
{
	DECLARE_CLASS_SIMPLE(CFSprite, Panel);

public:
	CFSprite(Panel *parent, const char *panelName);

	void SetSprite(const char *fileName, int count, float updateTime);

protected:
	virtual void PaintBackground(void);
	virtual void Paint(void);
	virtual void PaintBorder(void);
	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void ApplySchemeSettings(IScheme *pScheme);

private:
	CUtlVector<tex_t *>	_image;
	int					_alpha;
	int					_count;
	float				_startTime;
	float				_updateTime;
	uint32				_frame;
};

}

#endif